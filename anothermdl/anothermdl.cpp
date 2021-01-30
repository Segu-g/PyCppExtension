#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "../extension_utils/error_wrapper.hpp"
#include "cext.hpp"

namespace another{

    PyObject* another_refcnt(PyObject* module, PyObject* obj){
        mdl::CextCAPIManager* capsule = static_cast<mdl::CextCAPIManager*>(PyModule_GetState(module));
        return (capsule->function<mdl::API::cext_refcnt>())(module, obj);
    }


    PyMethodDef keywdarg_methods[] = {
        {"refcnt", reinterpret_cast<PyCFunction>(another_refcnt), METH_O,
         PyDoc_STR("This function uses the C API of the cext module")},
        {NULL, NULL, 0, NULL}
    };


    PyModuleDef anothermdl_module = {
        PyModuleDef_HEAD_INIT, // m_base
        "anothermdl", //m_name
        PyDoc_STR("This text is doccumentation of anothermdl."), //m_doc
        sizeof(mdl::CextCAPIManager),  // m_size
        keywdarg_methods, // m_methods
        nullptr, // m_reload
        nullptr, // m_traverse
        nullptr, // m_clear
        nullptr // m_free
    };

    PyMODINIT_FUNC
    PyInit_anothermdl(void)
    {
        PyObject* module;
        module = PyModule_Create(&anothermdl_module);
        if (module == nullptr){
            return nullptr;
        }
        try{
            mdl::CextCAPIManager* capsule = static_cast<mdl::CextCAPIManager*>(PyModule_GetState(module));
            if (capsule == nullptr){
                PyErr_NoMemory();
                return nullptr;
            }
            capsule = new (capsule) mdl::CextCAPIManager("mdl.cext._C_API");
            capsule->import();
        }catch (utils::ExtException& exception){
            exception.set_err();
            return nullptr;
        }
        return module;
    }



}
