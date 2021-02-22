#include "cext.hpp"

namespace mdl {

    PyMethodDef MyInt_methods[] = {
        {"add", (PyCFunction)MyInt_add, METH_O,
            PyDoc_STR("add(self, int) -> void")},
        {"__getstate__", (PyCFunction)MyInt_getstate, METH_NOARGS,
            PyDoc_STR("__getstate__")},
        {"__setstate__", (PyCFunction)MyInt_setstate, METH_VARARGS,
            PyDoc_STR("__setstate__")},
        {nullptr, nullptr}
    };

    PyNumberMethods MyInt_number_methods = {
        nullptr, // nb_add
        nullptr, // nb_multiply
        nullptr, // nb_subtract
        nullptr, // nb_remainder
        nullptr, // nb_divmod
        nullptr, // nb_power
        nullptr, // nb_negative
        nullptr, // nb_positive
        nullptr, // nb_absolute
        nullptr, // nb_bool
        nullptr, // nb_invert
        nullptr, // nb_lshift
        nullptr, // nb_rshift
        nullptr, // nb_and
        nullptr, // nb_xor
        nullptr, // nb_or
        (unaryfunc)MyInt_int, // nb_int
        nullptr, // *nb_reserved
        nullptr, // nb_float
        nullptr, // nb_inplace_add
        nullptr, // nb_inplace_subtract
        nullptr, // nb_inplace_multiply
        nullptr, // nb_inplace_remainder
        nullptr, // nb_inplace_power
        nullptr, // nb_inplace_lshift
        nullptr, // nb_inplace_rshift
        nullptr, // nb_inplace_and
        nullptr, // nb_inplace_xor
        nullptr, // nb_inplace_or
        nullptr, // nb_floor_divide
        nullptr, // nb_true_divide
        nullptr, // nb_inplace_floor_divide
        nullptr, // nb_inplace_true_divide
        nullptr, // nb_index
        nullptr, // nb_matrix_multiply
        nullptr, // nb_inplace_matrix_multiply
    };


    PyTypeObject MyInt_Type = {
        PyVarObject_HEAD_INIT(nullptr, 0)
        "mdl.cext.MyInt", // tp_name
        sizeof(MyInt_Object), // tp_basicsize
        0, // tp_itemsize
        // methods
        (destructor)MyInt_dealloc, // tp_dealloc
        0, // tp_vectorcall_offset
        nullptr, // tp_getattr
        nullptr, // tp_setattr
        nullptr, // tp_as_async
        nullptr, // tp_repr
        &MyInt_number_methods, // tp_as_number
        nullptr, // tp_as_sequence
        nullptr, // tp_as_mapping
        nullptr, // tp_hash
        nullptr, // tp_call
        nullptr, // tp_str
        PyObject_GenericGetAttr, // tp_getattro
        PyObject_GenericSetAttr, // tp_setattro
        nullptr, // tp_as_buffer
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_LONG_SUBCLASS, // tp_flags
        PyDoc_STR("tp_doc of PyTypeObject MyInt_Type"), // tp_doc
        nullptr, // tp_traverse
        nullptr, // tp_clear
        nullptr, // tp_richcompare
        0, // tp_weaklistoffset
        nullptr, // tp_iter
        nullptr, // tp_iternext
        MyInt_methods, // tp_methods
        nullptr, // tp_members
        nullptr, // tp_getset
        nullptr, // tp_base
        nullptr, // tp_dict
        nullptr, // tp_descr_get
        nullptr, // tp_descr_set
        offsetof(MyInt_Object, dict), // tp_dictoffset
        (initproc)init_MyInt_Object, // tp_init
        nullptr, // tp_alloc
        (newfunc)new_MyInt_Object, // tp_new
        nullptr, // tp_free
        nullptr, // tp_is_gc
        nullptr, // tp_bases
        nullptr, // tp_mro
        nullptr, // tp_cache
        nullptr, // tp_subclasses
        nullptr, //tp_weaklist
        nullptr //tp_finalize
    };


    PyObject* new_MyInt_Object(PyTypeObject *subtype, PyObject *args, PyObject *keywds){
        MyInt_Object* self;
        self = (MyInt_Object*) subtype->tp_alloc(subtype, 0);
        // self = PyObject_New(MyInt_Object, subtype);
        if (self == nullptr) {
            PyErr_NoMemory();
            return nullptr;
        }
        self->dict = PyDict_Copy(subtype->tp_dict);
        if (self->dict == nullptr){
            PyErr_SetString(PyExc_RuntimeError, "Faild to get a PyDictObject of MyInt_Object.");
            return nullptr;
        }
        return (PyObject*) self;
    }

    int init_MyInt_Object(PyObject* self, PyObject* args, PyObject* keywds){
        int value;
        const char *format = "i";
        static char* keywdlist[] = {"value", nullptr};
        if (PyArg_ParseTupleAndKeywords(args, keywds, format, keywdlist, &value) == 0){
            return -1;
        }
        reinterpret_cast<MyInt_Object*>(self)->value = value;
        return 0;
    }

    PyObject* MyInt_getstate(PyObject* self, PyObject* args){
        MyInt_Object* _self = (MyInt_Object*)self;
        return Py_BuildValue("(iO)", _self->value, _self->dict);
    }

    PyObject* MyInt_setstate(PyObject* self, PyObject* args){
        MyInt_Object* _self = (MyInt_Object*)self;
        int value = 0;
        PyObject* dict = nullptr;
        if (!PyArg_ParseTuple(args, "(iO)", &value, &dict)){
            return nullptr;
        }

        _self->value = value;
        if (PyDict_Update(_self->dict, dict) == -1){
            return nullptr;
        }
        Py_INCREF(Py_None);
        return Py_None;
    }

    PyObject* MyInt_add(PyObject *self, PyObject *obj){
        try{
            if(PyObject_IsInstance(obj, (PyObject*)&MyInt_Type) == 0){
                throw utils::ExtException(PyExc_TypeError,
                    std::string("Invalid codec of the given string to be converted."));
            }
            reinterpret_cast<MyInt_Object*>(self)->value += ((MyInt_Object*)obj )->value;
            Py_INCREF(Py_None);
            return Py_None;
        } catch (utils::ExtException& exception){
            exception.set_err();
            return nullptr;
        }

    }


    PyObject* MyInt_int(PyObject *self){
        return Py_BuildValue("i", reinterpret_cast<MyInt_Object*>(self)->value);
    }

    void MyInt_dealloc(PyObject* self){
        PyTypeObject *tp = Py_TYPE(self);
        tp->tp_free(self);
        Py_DECREF(tp);
    }


    int cext_exec(PyObject* module){
        int ret_val = -1;
        try {
            if (PyType_Ready(&MyInt_Type) < 0) {
                throw std::runtime_error("Faild to initialize MyInt_Object.");
            }
            PyModule_AddObject(module, "MyInt", reinterpret_cast<PyObject *>(&MyInt_Type));

            void** PyCextAPI = reinterpret_cast<void**>(PyMem_RawMalloc(static_cast<int>(API::API_NUM) * sizeof(void*)));

            PyCextAPI[static_cast<int>(API::MyInt_add)] = reinterpret_cast<void*>(MyInt_add);
            PyCextAPI[static_cast<int>(API::MyInt_int)] = reinterpret_cast<void*>(MyInt_int);
            PyCextAPI[static_cast<int>(API::MyInt_getstate)] = reinterpret_cast<void*>(MyInt_getstate);
            PyCextAPI[static_cast<int>(API::MyInt_setstate)] = reinterpret_cast<void*>(MyInt_setstate);
            PyCextAPI[static_cast<int>(API::new_MyInt_Object)] = reinterpret_cast<void*>(new_MyInt_Object);
            PyCextAPI[static_cast<int>(API::init_MyInt_Object)] = reinterpret_cast<void*>(init_MyInt_Object);
            PyCextAPI[static_cast<int>(API::MyInt_dealloc)] = reinterpret_cast<void*>(MyInt_dealloc);
            PyCextAPI[static_cast<int>(API::cext_refcnt)] = reinterpret_cast<void*>(cext_refcnt);


            PyObject *c_api_object = PyCapsule_New(reinterpret_cast<void*>(PyCextAPI),
                "mdl.cext._C_API",
                [](PyObject* capsule){
                    void* ptr = PyCapsule_GetPointer(capsule, "mdl.cext._C_API");
                    PyMem_RawFree(ptr);
                });
            if (PyModule_AddObject(module, "_C_API", c_api_object) < 0) {
                Py_XDECREF(c_api_object);
                Py_DECREF(module);
                throw std::runtime_error("Faild to generate cext._C_API.");
            }
            ret_val = 0;
        }
        catch(const std::runtime_error& err) {
            PyErr_SetImportError(Py_BuildValue("s", err.what()),
                                 Py_BuildValue("s", "mdl.cext"),
                                 Py_BuildValue("s", "cext.cpp"));
        }
        return ret_val;
    }

    PyModuleDef_Slot cext_slots[] = {
        {Py_mod_exec, reinterpret_cast<void*>(cext_exec)},
        {0, nullptr},
    };

    PyObject* cext_refcnt(PyObject* module, PyObject* obj){
        return PyLong_FromSsize_t(Py_REFCNT(obj));
    }

    PyMethodDef cext_methods[] = {
        {"refcnt", (PyCFunction)cext_refcnt, METH_O,
            PyDoc_STR("Show a reference count of a given object.")},
        {nullptr, nullptr},
    };

    PyDoc_STRVAR(module_doc, "This text is dummy document of module");

    PyModuleDef cext_module = {
        PyModuleDef_HEAD_INIT,
        "mdl.cext", // m_name
        module_doc, // m_doc
        0, // m_size
        cext_methods, // m_methods
        cext_slots, // m_slots
        nullptr, // m_traverse
        nullptr, // m_clear
        nullptr // m_free
    };

    PyMODINIT_FUNC
    PyInit_cext(void) {
        return PyModuleDef_Init(&cext_module);
    }

}
