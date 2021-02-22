#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "../extension_utils/error_wrapper.hpp"
#include "cext.hpp"

namespace anothermdl{

    mdl::CextCAPIManager* cext_capsule = nullptr;

    PyObject* another_refcnt(PyObject* module, PyObject* obj){
        return (cext_capsule->function<mdl::API::cext_refcnt>())(module, obj);
    }


    PyMethodDef anothermdl_methods[] = {
        {"refcnt", reinterpret_cast<PyCFunction>(another_refcnt), METH_O,
         PyDoc_STR("This function uses the C API of the cext module")},
        {NULL, NULL, 0, NULL}
    };


    PyModuleDef anothermdl_module = {
        PyModuleDef_HEAD_INIT, // m_base
        "anothermdl", //m_name
        PyDoc_STR("This text is doccumentation of anothermdl."), //m_doc
        0,  // m_size
        anothermdl_methods, // m_methods
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
            cext_capsule = static_cast<mdl::CextCAPIManager*>(PyMem_RawMalloc(sizeof(mdl::CextCAPIManager)));
            if (cext_capsule == nullptr){
                PyErr_NoMemory();
                return nullptr;
            }
            new (cext_capsule) mdl::CextCAPIManager("mdl.cext._C_API");
            cext_capsule->import();
        }catch (utils::ExtException& exception){
            exception.set_err();
            return nullptr;
        }
        return module;
    }



}
