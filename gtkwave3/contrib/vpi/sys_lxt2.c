#include  "vpi_user.h"
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <assert.h>
#include  <time.h>
#include  <lxt2_write.h>
#include  <inttypes.h>

/* enable if this speeds up dumping on your simulator */
#undef LXT2_UNREGISTER
#undef REALS_SUPPORTED

#define XXX_UINT64 int64_t
#define XXX_UINT64_FMT "llu"


struct lt_info
{
  struct lt_info *next, *dump_chain;
  struct lxt2_wr_symbol *lxt2sym;
  s_vpi_value value;
  vpiHandle item;
  vpiHandle cb;
  int row, length, lrange, rrange;
  unsigned is_changed:1;
};

static int variable_cb (p_cb_data cause);


static char *dump_path = NULL;
struct lxt2_wr_trace *lxt2 = NULL;

static int dump_is_off = 0;

static struct lt_info *lt_list = 0;
static struct lt_info *lt_dump_list = 0;

static int dumpvars_status = 0;	/* 0:fresh 1:cb installed, 2:callback done */
static XXX_UINT64 dumpvars_time;

static XXX_UINT64
timerec_to_time64 (s_vpi_time * vt)
{
  XXX_UINT64 hi = vt->high;
  XXX_UINT64 lo = vt->low;

  return ((hi << 32) | lo);
}


static int
dump_header_pending (void)
{
  return dumpvars_status != 2;
}


static int
variable_cb_rosync (p_cb_data cause)
{
  struct t_vpi_time time;
  struct lt_info *info = (struct lt_info *) cause->user_data;
  p_vpi_time tim = cause->time;
  XXX_UINT64 now64 = timerec_to_time64 (tim);
  char *cached_val = NULL;
  int i;
  struct lt_info *a_info;
  s_vpi_value value;

  lxt2_wr_set_time64 (lxt2, now64);

  memset (&time, 0, sizeof (time));
  time.type = vpiSimTime;

  value.value.str = NULL;
  a_info = lt_dump_list;

  while (a_info)
    {
#ifdef REALS_SUPPORTED
      if (vpi_get (vpiType, a_info->item) == vpiRealVar)
	{
	  value.format = vpiRealVal;
	  vpi_get_value (a_info->item, &value);
	  lxt2_wr_emit_value_double (lxt2, a_info->lxt2sym, a_info->row,
				    value.value.real);
	}
      else
#endif
	{
	  value.format = vpiBinStrVal;
	  vpi_get_value (a_info->item, &value);

	  lxt2_wr_emit_value_bit_string (lxt2, a_info->lxt2sym, a_info->row,
					value.value.str);
	}

      a_info->is_changed = 0;

#ifdef LXT2_UNREGISTER
      {
	struct t_cb_data cb;

	memset (&cb, 0, sizeof (cb));

	cb.time = &time;
	cb.user_data = (char *) a_info;

	cb.value = &a_info->value;
	cb.obj = a_info->item;
	cb.reason = cbValueChange;
	cb.cb_rtn = variable_cb;

	a_info->value.format = value.format;
	a_info->cb = vpi_register_cb (&cb);
      }
#endif

      a_info = a_info->dump_chain;
    }

  lt_dump_list = NULL;

  return (0);
}


static void
install_rosync_cb (void)
{
  struct t_cb_data cb;
  struct t_vpi_time time;

  memset (&cb, 0, sizeof (cb));
  memset (&time, 0, sizeof (time));
  time.type = vpiSimTime;

  cb.time = &time;
  cb.reason = cbReadOnlySynch;
  cb.cb_rtn = variable_cb_rosync;
  cb.user_data = NULL;
  cb.obj = NULL;

  vpi_register_cb (&cb);
}


static int
variable_cb (p_cb_data cause)
{
  struct lt_info *info = (struct lt_info *) cause->user_data;
  p_vpi_time tim = cause->time;
  XXX_UINT64 now64 = timerec_to_time64 (tim);

  if (dump_is_off)
    return 0;
  if (dump_header_pending ())
    return 0;
  if (info->is_changed)
    return 0;

  if (!lt_dump_list)
    {
      install_rosync_cb ();
    }

  info->is_changed = 1;
  info->dump_chain = lt_dump_list;
  lt_dump_list = info;

#ifdef LXT2_UNREGISTER
  vpi_remove_cb (info->cb);
#endif

  return (0);
}


static int
dumpvars_cb (p_cb_data cause)
{
  if (dumpvars_status != 1)
    return 0;

  dumpvars_status = 2;

  dumpvars_time = timerec_to_time64 (cause->time);

  return 0;
}


static int
install_dumpvars_callback (void)
{
  struct t_cb_data cb;
  struct t_vpi_time time;

  if (dumpvars_status == 1)
    return 0;

  if (dumpvars_status == 2)
    {
      vpi_mcd_printf (1, "Error:"
		      " $dumpvars ignored,"
		      " previously called at simtime %" XXX_UINT64_FMT "\n",
		      dumpvars_time);
      return 1;
    }

  memset (&cb, 0, sizeof (cb));
  memset (&time, 0, sizeof (time));
  time.type = vpiSimTime;
  cb.time = &time;
  cb.reason = cbReadOnlySynch;
  cb.cb_rtn = dumpvars_cb;
  cb.user_data = NULL;
  cb.obj = NULL;

  vpi_register_cb (&cb);

  dumpvars_status = 1;
  return 0;
}


static int
end_of_sim_cb (p_cb_data cause)
{
  int i;

  if (lxt2)
    {
      lxt2_wr_close (lxt2);
      lxt2 = NULL;
    }

  return (0);
}


static void
open_dumpfile (void)
{
  struct t_cb_data cb;
  struct t_vpi_time vtime;
  int prec;

  if (dump_path == NULL)
    {
      dump_path = strdup ("dump.lxt2");
    }

  lxt2 = lxt2_wr_init (dump_path);

  if (lxt2 == NULL)
    {
      free (dump_path);
      dump_path = NULL;

      vpi_mcd_printf (1,
		      "LXT2 Error: Unable to open %s for output.\n",
		      dump_path);
      return;
    }

  free (dump_path);
  dump_path = NULL;

  prec = vpi_get (vpiTimePrecision, 0);
  lxt2_wr_set_timescale (lxt2, prec);
  lxt2_wr_set_initial_value (lxt2, 'x');
  lxt2_wr_set_compression_depth (lxt2, 9);
  lxt2_wr_set_maxgranule (lxt2, 256);

  memset (&cb, 0, sizeof (cb));
  memset (&vtime, 0, sizeof (vtime));

  vtime.type = vpiSimTime;
  cb.time = &vtime;
  cb.reason = cbEndOfSimulation;
  cb.cb_rtn = end_of_sim_cb;
  cb.user_data = NULL;
  cb.obj = NULL;

  vpi_register_cb (&cb);
}


static int
sys_dumpfile_compiletf (char *name)
{
  vpiHandle sys = vpi_handle (vpiSysTfCall, 0);
  vpiHandle argv = vpi_iterate (vpiArgument, sys);
  vpiHandle item;

  char *path;

  if (argv && (item = vpi_scan (argv)))
    {
      s_vpi_value value;

      if (vpi_get (vpiType, item) != vpiConstant
	  || vpi_get (vpiConstType, item) != vpiStringConst)
	{
	  vpi_mcd_printf (1,
			  "LXT2 Error:"
			  " %s parameter must be a string constant\n", name);
	  return 0;
	}

      value.format = vpiStringVal;
      vpi_get_value (item, &value);
      path = strdup (value.value.str);

      vpi_free_object (argv);
    }
  else
    {
      path = strdup ("dump.lxt2");
    }

  if (dump_path)
    {
      vpi_mcd_printf (1, "LXT2 Warning:"
		      " Overriding dumpfile path %s with %s\n",
		      dump_path, path);
      free (dump_path);
    }

  dump_path = path;

  return 0;
}


static int
sys_dumpfile_calltf (char *name)
{
  return 0;
}


static int
sys_dumpflush_calltf (char *name)
{
  if (lxt2)
    {
      lxt2_wr_flush (lxt2);
    }

  return 0;
}


static int
draw_module_type (vpiHandle item, int typ, char *str)
{
  vpiHandle iter = vpi_iterate (typ, item);
  vpiHandle net;
  const char *name;
  struct t_cb_data cb;
  struct lt_info *info;
  struct t_vpi_time time;

  while ((net = vpi_scan (iter)))
    {
      int siz;

      name = vpi_get_str (vpiFullName, net);

      info = calloc (1, sizeof (*info));

      if (vpi_get (vpiVector, net))
	{
	  s_vpi_value lvalue, rvalue;
	  vpiHandle lrange = vpi_handle (vpiLeftRange, net);
	  vpiHandle rrange = vpi_handle (vpiRightRange, net);

	  lvalue.value.integer = 0;
	  lvalue.format = vpiIntVal;
	  vpi_get_value (lrange, &lvalue);

	  rvalue.value.integer = 0;
	  rvalue.format = vpiIntVal;
	  vpi_get_value (rrange, &rvalue);

	  siz = vpi_get (vpiSize, net);

	  info->lrange = lvalue.value.integer;
	  info->rrange = rvalue.value.integer;
	}
      else
	{
	  siz = 1;
	  info->lrange = info->rrange = -1;
	}

      if (vpi_get (vpiType, net) == vpiRealVar)
	{
#ifdef REALS_SUPPORTED
	  info->lxt2sym =
	    lxt2_wr_symbol_add (lxt2, name, 0, 0, 0, LXT2_WR_SYM_F_DOUBLE);
#else
	  free (info);
	  continue;
#endif
	}
      else
	{
	  info->lxt2sym =
	    lxt2_wr_symbol_add (lxt2, name, 0, info->lrange, info->rrange,
			       LXT2_WR_SYM_F_BITS);
	}

      info->item = net;
      info->is_changed = 0;
      info->length = siz;

      memset (&cb, 0, sizeof (cb));
      memset (&time, 0, sizeof (time));
      time.type = vpiSimTime;

      cb.time = &time;
      cb.user_data = (char *) info;

      cb.value = &info->value;
      cb.obj = net;
      cb.reason = cbValueChange;
      cb.cb_rtn = variable_cb;
      info->value.format = vpiObjTypeVal;
      info->next = lt_list;
      lt_list = info;

      info->cb = vpi_register_cb (&cb);
    }

  return (0);
}


static int
draw_module (vpiHandle item)
{
  draw_module_type (item, vpiNet, "net");
  draw_module_type (item, vpiReg, "reg");
  draw_module_type (item, vpiVariables, "var");

  return (0);
}


static int
draw_scope_lxt2 (vpiHandle item, int depth, int depth_max)
{
  vpiHandle orig = item;
  const char *name;
  char *type;

  if ((depth_max) && (depth >= depth_max))
    return (0);

  if (depth == 0)
    {
      name = vpi_get_str (vpiFullName, item);
      draw_module (item);
      draw_scope_lxt2 (item, depth + 1, depth_max);
    }
  else
    {
      int i;
      const int types[] = {
#if 0
	vpiFunction,
	vpiNamedBegin,
	vpiNamedFork,
	vpiTask,
#endif
	vpiModule,
	-1
      };

      for (i = 0; types[i] > 0; i++)
	{
	  vpiHandle iter = vpi_iterate (types[i], orig);
	  while ((item = vpi_scan (iter)))
	    {
	      name = vpi_get_str (vpiFullName, item);

	      draw_module (item);
	      draw_scope_lxt2 (item, depth + 1, depth_max);
	    }
	}
    }

  return (0);
}


/*
 * This function is also used in sys_lxt2 to check the arguments of the
 * lxt2 variant of $dumpvars.
 */
static int
sys_dumpvars_compiletf (char *name)
{
  vpiHandle sys = vpi_handle (vpiSysTfCall, 0);
  vpiHandle argv = vpi_iterate (vpiArgument, sys);
  vpiHandle tmp;

  if (argv == 0)
    return 0;

  tmp = vpi_scan (argv);
  assert (tmp);

  switch (vpi_get (vpiType, tmp))
    {
    case vpiConstant:
      if (vpi_get (vpiConstType, tmp) == vpiStringConst)
	{
	  vpi_printf ("ERROR: %s argument must be "
		      "a number constant.\n", name);
	  vpi_control (vpiFinish, 1);
	}
      break;

    case vpiNet:
    case vpiReg:
    case vpiIntegerVar:
      break;

    default:
      vpi_printf ("ERROR: %s argument must be " "a number constant.\n", name);
      vpi_control (vpiFinish, 1);
      break;
    }

  vpi_free_object (argv);
  return 0;
}


static int
sys_dumpvars_calltf (char *name)
{
  unsigned depth;
  s_vpi_value value;
  vpiHandle item = 0;
  vpiHandle sys = vpi_handle (vpiSysTfCall, 0);
  vpiHandle argv;

  if (lxt2 == 0)
    {
      open_dumpfile ();
      if (lxt2 == 0)
	return 0;
    }

  if (install_dumpvars_callback ())
    {
      return 0;
    }

  argv = vpi_iterate (vpiArgument, sys);

  depth = 0;
  if (argv && (item = vpi_scan (argv)))
    switch (vpi_get (vpiType, item))
      {
      case vpiConstant:
      case vpiNet:
      case vpiReg:
      case vpiIntegerVar:
	value.format = vpiIntVal;
	vpi_get_value (item, &value);
	depth = value.value.integer;
	break;
      }

  if (!argv)
    {
      vpiHandle parent = vpi_handle (vpiScope, sys);
      while (parent)
	{
	  item = parent;
	  parent = vpi_handle (vpiScope, item);
	}

    }
  else if (!item || !(item = vpi_scan (argv)))
    {
      item = vpi_handle (vpiScope, sys);
      argv = 0x0;
    }

  for (; item; item = argv ? vpi_scan (argv) : 0x0)
    {
      int dep = draw_scope_lxt2 (item, 0, depth);
    }


  {
    struct lt_info *a_info;
    s_vpi_value value;
    int siz_max = 1;
    char *siz_ptr = calloc (1, siz_max + 1);
    int i;
    char lrrange[33];
    int lrvalue;
    char precbuff[9];
    int prec = vpi_get (vpiTimePrecision, 0);

    for (i = 0; i < 8; i++)
      {
	precbuff[i] = '0' | ((prec >> (7 - i)) & 1);
      }
    precbuff[i] = 0;

    a_info = lt_list;
    while (a_info)
      {
	if (a_info->length > siz_max)
	  siz_max = a_info->length;
	a_info = a_info->next;
      }

    siz_ptr = calloc (1, siz_max + 1);

    for (i = 0; i < siz_max; i++)
      {
	siz_ptr[i] = 'x';
      }

    install_rosync_cb ();

    a_info = lt_list;
    lrrange[32] = 0;
    while (a_info)
      {
#ifdef REALS_SUPPORTED
	if (vpi_get (vpiType, a_info->item) == vpiRealVar)
	  {
	    /* nothing */
	  }
	else
#endif
	  {
	    lxt2_wr_emit_value_bit_string (lxt2, a_info->lxt2sym, a_info->row,
					  siz_ptr);
	  }

	a_info->is_changed = 1;
	a_info->dump_chain = lt_dump_list;
	lt_dump_list = a_info;

	a_info = a_info->next;
      }

    free (siz_ptr);
  }

  return 0;
}


static int
sys_dumpoff_compiletf (char *name)
{
  return (0);
}


static int
sys_dumpoff_calltf (char *name)
{
  dump_is_off = 1;

  return (0);
}


void
sys_lxt2_register ()
{
  s_vpi_systf_data tf_data;

  tf_data.type = vpiSysTask;
  tf_data.tfname = "$lxt2dumpfile";
  tf_data.calltf = sys_dumpfile_calltf;
  tf_data.compiletf = sys_dumpfile_compiletf;
  tf_data.sizetf = 0;
  tf_data.user_data = "$lxt2dumpfile";
  vpi_register_systf (&tf_data);

  tf_data.type = vpiSysTask;
  tf_data.tfname = "$lxt2dumpvars";
  tf_data.calltf = sys_dumpvars_calltf;
  tf_data.compiletf = sys_dumpvars_compiletf;
  tf_data.sizetf = 0;
  tf_data.user_data = "$lxt2dumpvars";
  vpi_register_systf (&tf_data);

  tf_data.type = vpiSysTask;
  tf_data.tfname = "$lxt2dumpoff";
  tf_data.calltf = sys_dumpoff_calltf;
  tf_data.compiletf = sys_dumpoff_compiletf;
  tf_data.sizetf = 0;
  tf_data.user_data = "$lxt2dumpoff";
  vpi_register_systf (&tf_data);
}
