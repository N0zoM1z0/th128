#include "Chain.hpp"

#include "Supervisor.hpp"

void __cdecl ChainDebugLog(const char *, ...)
{
}

ChainElem::ChainElem()
{
    flags &= ~1u;
    callback = 0;
    onRegistration = 0;
    onDeletion = 0;
    priority = 0;
    node.owner = this;
    node.next = 0;
    node.prev = 0;
}

ChainElem::ChainElem(ChainCallback fn)
{
    flags &= ~1u;
    callback = fn;
    onRegistration = 0;
    onDeletion = 0;
    priority = 0;
    node.owner = this;
    node.next = 0;
    node.prev = 0;
}

ChainElem::~ChainElem()
{
    callback = 0;
    onRegistration = 0;
    onDeletion = 0;
}

Chain::Chain()
{
    isShutdown = 0;
}

int Chain::RegisterCalc(ChainElem *elem, int priority)
{
    int result = 0;
    Chain *chain = g_Chain;
    if (elem->onRegistration != 0)
    {
        result = elem->onRegistration(elem->arg);
        elem->onRegistration = 0;
    }
    g_Supervisor.EnterCriticalSection(0);
    ChainNode *cursor = &chain->calcChain.node;
    elem->priority = priority;
    while (cursor->next != 0)
    {
        if (cursor->next->owner->priority >= priority)
        {
            break;
        }
        cursor = cursor->next;
    }
    ChainNode *node = &elem->node;
    if (cursor->next != 0)
    {
        node->next = cursor->next;
        cursor->next->prev = node;
    }
    cursor->next = node;
    node->prev = cursor;
    g_Supervisor.LeaveCriticalSection(0);
    return result;
}

int Chain::RegisterDraw(ChainElem *elem, int priority)
{
    int result = 0;
    Chain *chain = g_Chain;
    if (elem->onRegistration != 0)
    {
        result = elem->onRegistration(elem->arg);
        elem->onRegistration = 0;
    }
    g_Supervisor.EnterCriticalSection(0);
    ChainNode *cursor = &chain->drawChain.node;
    elem->priority = priority;
    while (cursor->next != 0)
    {
        if (cursor->next->owner->priority >= priority)
        {
            break;
        }
        cursor = cursor->next;
    }
    ChainNode *node = &elem->node;
    if (cursor->next != 0)
    {
        node->next = cursor->next;
        cursor->next->prev = node;
    }
    cursor->next = node;
    node->prev = cursor;
    g_Supervisor.LeaveCriticalSection(0);
    return result;
}

int Chain::RunCalcChain()
{
    g_Supervisor.EnterCriticalSection(0);
    int count;
restart:
    ChainNode *node = calcChain.node.next;
    count = 0;
    while (node != 0)
    {
        current = node->next;
        ChainElem *elem = node->owner;
        if (elem->callback != 0)
        {
            if ((elem->flags & 2u) != 0)
            {
            retry:
                if (isShutdown != 0)
                {
                    goto on_delete;
                }
                g_Supervisor.LeaveCriticalSection(0);
                int status = elem->callback(elem->arg);
                g_Supervisor.EnterCriticalSection(0);
                switch (status)
                {
                case 2:
                    if ((elem->flags & 2u) != 0)
                    {
                        goto retry;
                    }
                    ++count;
                    node = current;
                    continue;
                case 0:
                    CutImpl(elem);
                    ++count;
                    node = current;
                    continue;
                case 7:
                on_delete:
                    if (elem->onDeletion != 0)
                    {
                        elem->onDeletion(elem->arg);
                    }
                    break;
                case 4:
                case 8:
                    count = 0;
                    goto done;
                case 3:
                    count = 1;
                    goto done;
                case 5:
                    count = -1;
                    goto done;
                case 6:
                    goto restart;
                case 1:
                default:
                    break;
                }
            }
            ++count;
        }
        node = current;
    }
done:
    g_Supervisor.LeaveCriticalSection(0);
    return count;
}

int Chain::RunDrawChain()
{
    Chain *chain = g_Chain;
    g_Supervisor.EnterCriticalSection(0);
    ChainNode *node = chain->drawChain.node.next;
    int count = 0;
    while (node != 0)
    {
        chain->current = node->next;
        ChainElem *elem = node->owner;
        if (elem->callback != 0)
        {
            if ((elem->flags & 2u) != 0)
            {
            retry:
                g_Supervisor.LeaveCriticalSection(0);
                int status = elem->callback(elem->arg);
                g_Supervisor.EnterCriticalSection(0);
                switch (status)
                {
                case 2:
                    if ((elem->flags & 2u) != 0)
                    {
                        goto retry;
                    }
                    break;
                case 0:
                    chain->CutImpl(elem);
                    break;
                case 4:
                    count = 0;
                    goto done;
                case 3:
                    count = 1;
                    goto done;
                case 5:
                    count = -1;
                    goto done;
                case 1:
                default:
                    break;
                }
            }
            ++count;
        }
        node = chain->current;
    }
done:
    g_Supervisor.LeaveCriticalSection(0);
    return count;
}

void Chain::CutList(ChainElem *sentinel)
{
    ChainNode *node = sentinel->node.next;
    while (node != 0)
    {
        ChainElem *elem = node->owner;
        ChainNode *nextNode = node->next;
        if (elem != 0)
        {
            g_Supervisor.EnterCriticalSection(0);
            CutImpl(elem);
            g_Supervisor.LeaveCriticalSection(0);
        }
        node = nextNode;
    }
}

void Chain::Release()
{
    g_Supervisor.thread.Stop();
    isShutdown = 1;
    RunCalcChain();
    CutList(&calcChain);
    CutList(&drawChain);
}

ChainElem *Chain::CreateElem(ChainCallback fn)
{
    ChainElem *elem = new ChainElem();
    elem->flags |= 1u;
    elem->SetCallback(fn);
    return elem;
}

void Chain::Cut(ChainElem *elem)
{
    if (elem != 0)
    {
        g_Supervisor.EnterCriticalSection(0);
        CutImpl(elem);
        g_Supervisor.LeaveCriticalSection(0);
    }
}

void Chain::CutImpl(ChainElem *elem)
{
    if (elem == 0)
    {
        return;
    }
    ChainNode *node = &calcChain.node;
    while (node != 0)
    {
        if (node->owner == elem)
        {
            goto found;
        }
        node = node->next;
    }
    node = &drawChain.node;
    while (node != 0)
    {
        if (node->owner == elem)
        {
            goto found;
        }
        node = node->next;
    }
    return;

found:
    if (current == node)
    {
        current = node->next;
    }
    if (node->prev == 0)
    {
        return;
    }
    if (node->next != 0)
    {
        node->next->prev = node->prev;
    }
    if (node->prev != 0)
    {
        node->prev->next = node->next;
    }
    node->next = 0;
    node->prev = 0;
    elem->callback = 0;
    if ((elem->flags & 1u) != 0)
    {
        delete elem;
    }
}

ChainElem *Chain::CreateAndRegisterCalcImmediate(ChainCallback fn, void *arg, int priority)
{
    ChainElem *elem = new ChainElem();
    elem->flags |= 3u;
    elem->onRegistration = 0;
    elem->onDeletion = 0;
    elem->callback = fn;
    elem->arg = arg;
    RegisterCalc(elem, priority);
    return elem;
}

ChainElem *Chain::CreateAndRegisterDrawImmediate(ChainCallback fn, void *arg, int priority)
{
    ChainElem *elem = new ChainElem();
    elem->flags |= 3u;
    elem->onRegistration = 0;
    elem->onDeletion = 0;
    elem->callback = fn;
    elem->arg = arg;
    RegisterDraw(elem, priority);
    return elem;
}

ChainElem *Chain::CreateAndRegisterCalc(ChainCallback fn, void *arg, int priority)
{
    ChainElem *elem = new ChainElem();
    elem->SetCallback(fn);
    elem->arg = arg;
    elem->flags = (elem->flags & ~2u) | 1u;
    RegisterCalc(elem, priority);
    return elem;
}

ChainElem *Chain::CreateAndRegisterDraw(ChainCallback fn, void *arg, int priority)
{
    ChainElem *elem = new ChainElem();
    elem->SetCallback(fn);
    elem->arg = arg;
    elem->flags = (elem->flags & ~2u) | 1u;
    RegisterDraw(elem, priority);
    return elem;
}
