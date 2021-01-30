#include "error_wrapper.hpp"

namespace utils {
    ExtException::ExtException(){
        this->exception = PyExc_RuntimeError;
        Py_INCREF(Py_None);
        this->value = Py_None;
    }

    ExtException::ExtException(PyObject* exception){
        this->exception = exception;
        Py_INCREF(Py_None);
        this->value = Py_None;
    }

    ExtException::ExtException(PyObject* exception, PyObject* value){
        Py_INCREF(value);
        this->exception = exception;
        this->value = value;
    }

    ExtException::ExtException(PyObject* exception, std::string text){
        this->exception = exception;
        const char* error = "Faild to convert the given c string to python object.";
        PyObject* value = PyUnicode_DecodeASCII(text.c_str(), text.size(), error);
        if (value == nullptr){
            PyErr_Print();
            PyErr_Clear();
            Py_INCREF(Py_None);
            this-> value = Py_None;
        } else{
            this->value = value;
        }
    }

    ExtException::~ExtException(){
        Py_DECREF(this->value);
    }

    const char* ExtException::what(){
        PyObject* unicode = PyObject_Str(this->value);
        PyObject* ascii = PyUnicode_AsASCIIString(unicode);
        const char* ascii_cstr = PyBytes_AsString(ascii);
        Py_DECREF(unicode);
        Py_DECREF(ascii);
        return ascii_cstr;
    }

    void ExtException::set_err(){
        PyErr_SetObject(this->exception, this->value);
        return;
    }
}
