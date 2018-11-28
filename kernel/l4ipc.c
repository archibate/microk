// vim: fdm=marker
#ifdef fucker // {{{

// https://github.com/jserv/codezero/blob/master/docs/Codezero_Reference_Manual.pdf
l4_send   (to,   flags) := l4_ipc(to,   L4_NIL, flags);
l4_recieve(from, flags) := l4_ipc(L4_NIL, from, flags);

l4_ipc(to, from, flags) := { l4_send(to, flags); l4_recieve(from, flags); }

#endif // }}}

struct capability { // means resource
	l4id_t owner;
};

ulong l4_ipc(l4id_t to, l4id_t from, uint flags)
{
}

ulong l4_map(void *p, void *v, uint pages, uint flags, l4id_t tid)
{
}

ulong l4_unmap(void *v, uint pages, l4id_t tid)
{
}
