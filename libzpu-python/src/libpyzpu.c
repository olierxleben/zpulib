#include <Python.h>

#include <zpu.h>
#include <stdio.h>

static PyObject* pyzpu_load_from_file(PyObject* self, PyObject* args, PyObject* keywds);

static PyMethodDef zpu_methods[] = {
	{"load_from_file", pyzpu_load_from_file, METH_VARARGS|METH_KEYWORDS, "Load a new program into the ZPU"},
	{NULL, NULL, 0, NULL}
};

static PyObject* pyzpu_load_from_file(PyObject* self, PyObject* args, PyObject* keywds)
{
	static char* kwlist[] = {"filename", NULL};
	const  char* filename;
	
	if (!PyArg_ParseTupleAndKeywords(args, keywds, "s", kwlist, &(filename)))
	{
		return NULL;
	}
	
	if (zpu_load_from_file((char*) filename) != ZPU_ERR_OK)
	{
		PyErr_SetString(PyExc_RuntimeError, zpu_error());
		return NULL;
	}
	
	Py_RETURN_NONE;
}

PyMODINIT_FUNC initzpu()
{
	Py_InitModule("zpu", zpu_methods);
}
