#include "e_mod_main.h"

/* gadcon requirements */
static E_Gadcon_Client *_gc_init(E_Gadcon * gc, const char *name, const char *id, const char *style);
static void             _gc_shutdown(E_Gadcon_Client * gcc);
static void             _gc_orient(E_Gadcon_Client * gcc, E_Gadcon_Orient orient);
static const char      *_gc_label(const E_Gadcon_Client_Class *client_class);
static                  Evas_Object *_gc_icon(const E_Gadcon_Client_Class *client_class, Evas * evas);
static const char      *_gc_id_new(const E_Gadcon_Client_Class *client_class);

/* and actually define the gadcon class that this module provides (just 1) */
static const E_Gadcon_Client_Class _gadcon_class = {
   GADCON_CLIENT_CLASS_VERSION,
   "wincontrol",
   {
      _gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon, _gc_id_new, NULL,
      e_gadcon_site_is_not_toolbar
   },
   E_GADCON_CLIENT_STYLE_PLAIN
};

/* actual module specifics */
typedef struct _Instance Instance;
struct _Instance
{
   E_Gadcon_Client *gcc;
   Evas_Object *o_button;
   Evas_Object *bd;
};

static void _button_cb_mouse_down(void *data, Evas * e, Evas_Object * obj, void *event_info);
static void _button_cb_mouse_wheel(void *data, Evas * e, Evas_Object * obj, void *event_info);

static E_Module *wincontrol_module = NULL;
Eina_List *handlers;

static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style)
{
   Evas_Object *o;
  
   E_Gadcon_Client *gcc;
   Instance *inst;
   char buf[PATH_MAX];

   inst = E_NEW(Instance, 1);

   snprintf(buf, sizeof(buf), "%s/e-module-wincontrol.edj", e_module_dir_get(wincontrol_module));
   
   o = edje_object_add(gc->evas);
   //~ e_icon_fdo_icon_set(o, "window-close");
   if (!e_theme_edje_object_set(o, "base/theme/modules/wincontrol", "modules/wincontrol/main"))
     edje_object_file_set(o, buf, "modules/wincontrol/main");

   evas_object_show(o);

   gcc = e_gadcon_client_new(gc, name, id, style, o);
   gcc->data = inst;

   inst->gcc = gcc;
   inst->o_button = o;

   e_gadcon_client_util_menu_attach(gcc);

   evas_object_event_callback_add (o, EVAS_CALLBACK_MOUSE_DOWN,
                                   _button_cb_mouse_down, inst);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_WHEEL,
                                  _button_cb_mouse_wheel, inst);                                   

   return gcc;
}

static void
_gc_shutdown(E_Gadcon_Client *gcc)
{
   Instance *inst;

   inst = gcc->data;
   while (handlers) 
     {
        ecore_event_handler_del(handlers->data);
        handlers = eina_list_remove_list(handlers, handlers);
     }

   evas_object_del(inst->o_button);
   E_FREE(inst);
}

static void
_gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient)
{
   e_gadcon_client_aspect_set (gcc, 16, 16);
   e_gadcon_client_min_size_set (gcc, 16, 16);
}

static const char *
_gc_label (const E_Gadcon_Client_Class *client_class)
{
   return "wincontrol";
}

static Evas_Object *
_gc_icon(const E_Gadcon_Client_Class *client_class, Evas * evas)
{
   Evas_Object *o;
   char buf[PATH_MAX];

   o = edje_object_add(evas);
   snprintf (buf, sizeof(buf), "%s/e-module-wincontrol.edj", e_module_dir_get(wincontrol_module));
   edje_object_file_set(o, buf, "icon");
   return o;
}

static const char *
_gc_id_new (const E_Gadcon_Client_Class *client_class)
{
   return _gadcon_class.name;
}

static void
_button_cb_mouse_down (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  Evas_Event_Mouse_Down *ev;
  
  ev = event_info;
  if (ev->button == 1)
  {
     E_Object * obj;
     
     if (!obj) obj = E_OBJECT(e_border_focused_get());
     if (!obj) return;
     if (obj->type != E_BORDER_TYPE)
     {
       obj = E_OBJECT(e_border_focused_get());
       if (!obj) return;
     }
     if (!((E_Border *)obj)->lock_close)
       e_border_act_close_begin((E_Border *)obj);
  }
}

static void
_button_cb_mouse_wheel (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Wheel *ev = event_info;
   
   if (ev->z > 0)
   {
     E_Object * obj;
     if (!obj) obj = E_OBJECT(e_border_focused_get());
     if (!obj) return;
     if (obj->type != E_BORDER_TYPE)
     {
       obj = E_OBJECT(e_border_focused_get());
       if (!obj) return;
     }
     if (!((E_Border *)obj)->lock_user_iconify)
     {
       E_Border *bd;

       bd = (E_Border *)obj;
       e_border_iconify(bd);
     }
   }
   else if (ev->z < 0)
   {
     E_Object * obj;
     E_Border *bd;
     Eina_Bool resize = EINA_FALSE;

     if (!obj) obj = E_OBJECT(e_border_focused_get());
     if (!obj) return;
     if (obj->type != E_BORDER_TYPE)
     {
        obj = E_OBJECT(e_border_focused_get());
        if (!obj) return;
     }
     bd = (E_Border *)obj;

     if (bd->internal && (bd->client.netwm.type == ECORE_X_WINDOW_TYPE_DIALOG))
       resize = (bd->client.icccm.max_w != bd->client.icccm.min_w);
     if ((!bd->lock_user_maximize) && (!bd->fullscreen) &&
        (resize || (bd->client.netwm.type == ECORE_X_WINDOW_TYPE_NORMAL) ||
        (bd->client.netwm.type == ECORE_X_WINDOW_TYPE_UNKNOWN)))
     {
         if ((bd->maximized & E_MAXIMIZE_TYPE) != E_MAXIMIZE_NONE)
         {
            E_Maximize max;

            max = E_MAXIMIZE_BOTH;
            max &= E_MAXIMIZE_DIRECTION;
            if (max == E_MAXIMIZE_VERTICAL)
            {
              if (bd->maximized & E_MAXIMIZE_VERTICAL)
                    e_border_unmaximize(bd, E_MAXIMIZE_VERTICAL);
              else
                    goto maximize;
            }
            else if (max == E_MAXIMIZE_HORIZONTAL)
            {
              if (bd->maximized & E_MAXIMIZE_HORIZONTAL)
                    e_border_unmaximize(bd, E_MAXIMIZE_HORIZONTAL);
              else
                    goto maximize;
            }
            else if (max == E_MAXIMIZE_LEFT)
            {
              if (bd->maximized & E_MAXIMIZE_LEFT)
                    e_border_unmaximize(bd, E_MAXIMIZE_LEFT);
              else
                    goto maximize;
            }
            else if (max == E_MAXIMIZE_RIGHT)
            {
              if (bd->maximized & E_MAXIMIZE_RIGHT)
                    e_border_unmaximize(bd, E_MAXIMIZE_RIGHT);
              else
                    goto maximize;
            }
            else
                e_border_unmaximize(bd, E_MAXIMIZE_BOTH);
         }
         else
         {
maximize:
         {
            E_Maximize max;

            max = E_MAXIMIZE_BOTH;
            max |= E_MAXIMIZE_EXPAND;
            e_border_maximize(bd,  max);
         }
         }
      }
   }   
}

/* module setup */
EAPI E_Module_Api e_modapi = {
  E_MODULE_API_VERSION,
  "wincontrol"
};

EAPI void *
e_modapi_init (E_Module * m)
{
   wincontrol_module = m;
   e_gadcon_provider_register(&_gadcon_class);
   return wincontrol_module;
}

EAPI int
e_modapi_shutdown (E_Module * m)
{
  wincontrol_module = NULL;
  e_gadcon_provider_unregister(&_gadcon_class);
  return 1;
}

EAPI int
e_modapi_save(E_Module * m)
{
  return 1;
}
