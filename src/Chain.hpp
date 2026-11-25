#ifndef TH128_CHAIN_HPP
#define TH128_CHAIN_HPP

typedef int(__fastcall *ChainCallback)(void *);

struct ChainElem;

struct ChainNode
{
    ChainElem *owner; // +0x00
    ChainNode *next;  // +0x04
    ChainNode *prev;  // +0x08
};

struct ChainElem
{
    int priority;                 // +0x00
    unsigned int flags;           // +0x04 (bit 0 = heap allocated, bit 1 = enabled)
    ChainCallback callback;       // +0x08
    ChainCallback onRegistration; // +0x0C
    ChainCallback onDeletion;     // +0x10
    ChainNode node;               // +0x14
    void *arg;                    // +0x20

    ChainElem();
    explicit ChainElem(ChainCallback fn);
    ~ChainElem();

    void SetCallback(ChainCallback fn)
    {
        callback = fn;
        onRegistration = 0;
        onDeletion = 0;
    }
};

class Chain
{
public:
    Chain();

    int RegisterCalc(ChainElem *elem, int priority);
    int RegisterDraw(ChainElem *elem, int priority);
    int RunCalcChain();
    int RunDrawChain();
    void CutList(ChainElem *sentinel);
    void Release();
    ChainElem *CreateElem(ChainCallback fn);
    void Cut(ChainElem *elem);
    void CutImpl(ChainElem *elem);
    ChainElem *CreateAndRegisterCalcImmediate(ChainCallback fn, void *arg, int priority);
    ChainElem *CreateAndRegisterDrawImmediate(ChainCallback fn, void *arg, int priority);
    ChainElem *CreateAndRegisterCalc(ChainCallback fn, void *arg, int priority);
    ChainElem *CreateAndRegisterDraw(ChainCallback fn, void *arg, int priority);

    ChainElem calcChain; // +0x00
    ChainElem drawChain; // +0x24
    ChainNode *current;  // +0x48
    int isShutdown;      // +0x4C
};

extern Chain *g_Chain;

void __cdecl ChainDebugLog(const char *fmt, ...);

#endif
