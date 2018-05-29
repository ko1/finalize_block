#include "ruby/ruby.h"
#include "ruby/debug.h"

static VALUE c_FinalizeBlock;
static struct st_table *g_allocated_objects;

static void
newobj_i(VALUE tpval, void *data)
{
    rb_trace_arg_t *tparg = rb_tracearg_from_tracepoint(tpval);
    VALUE obj = rb_tracearg_object(tparg);
    VALUE klass = rb_obj_class(obj);
    VALUE target_class = (VALUE)data;

    if (klass == target_class) {
        st_insert(g_allocated_objects, (st_data_t)obj, 1);
    }
}

static void
freeobj_i(VALUE tpval, void *data)
{
    rb_trace_arg_t *tparg = rb_tracearg_from_tracepoint(tpval);
    VALUE obj = rb_tracearg_object(tparg);
    st_data_t key, val;

    if (st_lookup(g_allocated_objects, (st_data_t)obj, &val)) {
        st_delete(g_allocated_objects, &key, &val);
    }
}

static VALUE
setup_traces(VALUE self, VALUE klass)
{
    VALUE traces = rb_ary_new();
    rb_ary_push(traces, rb_tracepoint_new(0, RUBY_INTERNAL_EVENT_NEWOBJ, newobj_i, (void *)klass));
    rb_ary_push(traces, rb_tracepoint_new(0, RUBY_INTERNAL_EVENT_FREEOBJ, freeobj_i, NULL));
    return traces;
}

static int
each_target_objects_i(st_data_t key, st_data_t val, st_data_t data)
{
    VALUE obj = (VALUE)key;
    rb_yield(obj);
    return ST_CONTINUE;
}

static VALUE
each_target_objects(VALUE self)
{
    st_foreach(g_allocated_objects, each_target_objects_i, 0);
    return self;
}

void
Init_finalize_block(void)
{
    c_FinalizeBlock = rb_define_class("FinalizeBlock", rb_cObject);
    rb_define_method(c_FinalizeBlock, "setup_traces", setup_traces, 1);
    rb_define_method(c_FinalizeBlock, "each_target_objects", each_target_objects, 0);
    g_allocated_objects = st_init_numtable();
}
