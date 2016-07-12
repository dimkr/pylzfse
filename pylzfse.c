#include <lzfse.h>
#include <Python.h>

static PyObject *LzfseError;

static PyObject*
lzfse_op(PyObject* self,
         PyObject* args,
         size_t (*op)(uint8_t *__restrict,
                      size_t,
                      const uint8_t *__restrict,
                      size_t,
                      void *__restrict),
         size_t (*get_outlen)(const size_t),
         size_t (*get_auxlen)())
{
    PyObject *str;
    const char *in;
    char *out;
    void *aux;
    int inlen;
    size_t outlen;

    if (!PyArg_ParseTuple(args, "s#", &in, &inlen))
        return NULL;

    outlen = get_outlen((size_t)inlen);
    out = (char *)malloc(outlen + 1);
    if (!out)
        return PyErr_NoMemory();

    aux = malloc(get_auxlen());
    if (!aux) {
        free(out);
        return PyErr_NoMemory();
    }

    outlen = op((uint8_t *)out,
                outlen - 1,
                (const uint8_t *)in,
                (size_t)inlen,
                aux);
    free(aux);

    if (!outlen) {
        free(out);
        PyErr_SetNone(LzfseError);
        return NULL;
    }

    out[outlen] = '\0';
    str = PyString_FromStringAndSize(out, (Py_ssize_t)outlen);
    free(out);
    if (!str)
        PyErr_SetNone(LzfseError);
    return str;
}

static size_t
get_encode_outlen(const size_t inlen)
{
    return inlen;
}

static PyObject*
lzfse_compress(PyObject* self, PyObject* args)
{
    return lzfse_op(self,
                    args,
                    lzfse_encode_buffer,
                    get_encode_outlen,
                    lzfse_encode_scratch_size);
}

static size_t
get_decode_outlen(const size_t inlen)
{
    /* same assumption as lzfse_main.c */
    return inlen * 4;
}

static PyObject*
lzfse_decompress(PyObject* self, PyObject* args)
{
    return lzfse_op(self,
                    args,
                    lzfse_decode_buffer,
                    get_decode_outlen,
                    lzfse_decode_scratch_size);
}

static PyMethodDef LzfseMethods[] = {
    {"compress", lzfse_compress, METH_VARARGS, "Compress a buffer using LZFSE."},
    {"decompress", lzfse_decompress, METH_VARARGS, "Decompress a LZFSE-compressed buffer."},
    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC
initlzfse(void)
{
    PyObject *m;

    m = Py_InitModule("lzfse", LzfseMethods);
    if (!m)
        return;

    LzfseError = PyErr_NewException("lzfse.error", NULL, NULL);
    if (LzfseError) {
        Py_INCREF(LzfseError);
        PyModule_AddObject(m, "error", LzfseError);
    }
}
