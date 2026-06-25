// debuglib.dll - Freelancer / DAlib debug network library
//
// The first cluster of functions implement a small TCP "debug message" server
// object.  It is a multiple-inheritance class exposing two __stdcall interfaces,
// owns a listen socket, two intrusive message lists and an optional sink.
// Reconstructed from the original binary and matched with objdiff.

typedef unsigned int   SOCKET;
typedef unsigned short u_short;
typedef unsigned long  u_long;

#define INVALID_SOCKET ((SOCKET)(~0))

extern "C" void * __cdecl memset(void *dst, int val, unsigned int count);

// ---- Winsock imports (__stdcall) -------------------------------------------
extern "C" {
    SOCKET  __stdcall socket(int af, int type, int protocol);
    int     __stdcall setsockopt(SOCKET s, int level, int optname, const char *optval, int optlen);
    u_short __stdcall htons(u_short hostshort);
    int     __stdcall bind(SOCKET s, const struct sockaddr *name, int namelen);
    int     __stdcall listen(SOCKET s, int backlog);
    int     __stdcall closesocket(SOCKET s);
}

struct sockaddr {
    u_short sa_family;
    char    sa_data[14];
};

// ---- internal helpers (named after their original addresses) ---------------
extern "C" int  sub_65D1330(void);                   // report last winsock error
extern "C" void sub_65D6CB8(void *block);            // free()
extern "C" void __fastcall sub_65D1C20(void *self);  // ~DebugMessage

// A queued debug message.  Only the field used by the erase path is named.
struct DebugMessage {
    char  pad[0x14];
    int   id;            // +0x14
};

// Intrusive list node for the message queue (DebugServer+0x10, 12-byte node).
struct MsgNode {
    MsgNode      *next;  // +0x00
    MsgNode      *prev;  // +0x04
    DebugMessage *value; // +0x08
};

// Node for the second list (DebugServer+0x24, 16-byte node).
struct ConnNode {
    ConnNode *next;      // +0x00
    ConnNode *prev;      // +0x04
    char      value[8];  // +0x08
};

// Iterator returned by the erase helper; the user-declared copy constructor
// forces a hidden return-pointer, matching std::list<>::iterator.
struct MsgIter {
    MsgNode *node;
    MsgIter(MsgNode *n) : node(n) {}
    MsgIter(const MsgIter &o) : node(o.node) {}
};

// Notification sink: a C-style vtable whose entries take the sink as an
// explicit first argument (__stdcall).
struct Sink;
typedef void (__stdcall *SinkFn)(Sink *self, int code, int id);
struct Sink {
    SinkFn *vtbl;
};

// Two intrusive doubly-linked lists with a heap-allocated sentinel node.  The
// inlined constructor is what emits the operator-new calls in the server ctor;
// the destructor provides the cleanup the EH machinery unwinds to.
struct MsgList {
    char     flag;   // +0x00
    MsgNode *head;   // +0x04
    int      size;   // +0x08
    MsgList(char f)
    {
        flag = f;
        head = (MsgNode *)::operator new(sizeof(MsgNode));
        head->next = head;
        head->prev = head;
        size = 0;
    }
    ~MsgList();
};

struct ConnList {
    char      flag;  // +0x00
    ConnNode *head;  // +0x04
    int       size;  // +0x08
    ConnList(char f)
    {
        flag = f;
        head = (ConnNode *)::operator new(sizeof(ConnNode));
        head->next = head;
        head->prev = head;
        size = 0;
    }
    ~ConnList();
};

// The two __stdcall interfaces the server implements (content irrelevant to the
// matched code; only the two vftable pointers they contribute matter).
struct IDebugA {
    virtual void __stdcall a0() = 0;
    virtual ~IDebugA() {}
};
struct IDebugB {
    virtual void __stdcall b0() = 0;
};

struct DebugServer : IDebugA, IDebugB {
    SOCKET   sock;        // +0x08
    MsgList  list1;       // +0x0C  (flag@0C head@10 size@14)
    int      maxConn;     // +0x18
    int      port;        // +0x1C
    ConnList list2;       // +0x20  (flag@20 head@24 size@28)
    Sink    *sink;        // +0x2C

    DebugServer();                  // sub_65D1000
    virtual ~DebugServer();         // sub_65D10C0 / sub_65D10A0

    MsgIter EraseMessage(MsgNode *node);  // sub_65D11B0
};

// stubs so the EH machinery has a non-trivial destructor to unwind to; refined
// when matching the destructor itself.
MsgList::~MsgList() {}
ConnList::~ConnList() {}

// sub_65D1000 : construct the server.
DebugServer::DebugServer()
    : list1((char)maxConn), list2((char)maxConn)
{
    port = 0;
    sink = 0;
    sock = INVALID_SOCKET;
    maxConn = 4;
}

// sub_65D10C0 : destroy the server (placeholder body).
DebugServer::~DebugServer()
{
}

// sub_65D11B0 : remove one message node, free it, notify the sink, and return
// an iterator to the following node.
MsgIter DebugServer::EraseMessage(MsgNode *node)
{
    DebugMessage *value = node->value;
    int id = value->id;
    sub_65D1C20(value);
    sub_65D6CB8(value);

    MsgNode *nxt = node->next;
    node->prev->next = node->next;
    node->next->prev = node->prev;
    sub_65D6CB8(node);

    --this->list1.size;
    if (this->sink)
        this->sink->vtbl[3](this->sink, 1, id);

    return MsgIter(nxt);
}

// sub_65D1210 : bind + listen the server socket (__stdcall virtual, slot 3).
extern "C" int __stdcall sub_65D1210(DebugServer *self, u_short port, int maxConn)
{
    if (self->sock != INVALID_SOCKET)
        return -1;

    if (maxConn > 0 && maxConn <= 256)
        self->maxConn = maxConn;
    self->port = port;

    self->sock = socket(2, 1, 6);
    if (self->sock == INVALID_SOCKET)
    {
        sub_65D1330();
        return -1;
    }

    int optval = 1;
    if (setsockopt(self->sock, 6, 4, (const char *)&optval, 4) == -1)
        sub_65D1330();

    struct sockaddr name;
    memset(&name, 0, sizeof(name));
    name.sa_family = 2;
    *(u_short *)&name.sa_data[0] = htons((u_short)self->port);
    *(u_long *)&name.sa_data[2] = 0;

    if (bind(self->sock, &name, 16) == -1)
    {
        sub_65D1330();
        closesocket(self->sock);
        self->sock = INVALID_SOCKET;
        return -1;
    }
    if (listen(self->sock, 5) == -1)
    {
        sub_65D1330();
        closesocket(self->sock);
        self->sock = INVALID_SOCKET;
        return -1;
    }
    return 0;
}
