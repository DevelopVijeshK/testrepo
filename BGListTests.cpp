// Sri Ganeshji : Sri Balaji : Sri Pitreshwarji : Sri Durgaji : Sri Venkateshwara

#define TW_DIAG_FRIEND_PREFIX   Diag
#define TW_TEST_MODE 1

//TODO: #include    "TWCoreApplicationLib.hpp"

#include    "TWCoreDataLib.hpp"

eTWIterate
MyListIterateFunc (TWBaseElem * /*pElem*/, VPtr /*pParms*/) noexcept
{
        TRACEFUNC;
    TRACE_LOG ("In List Iterate Function\n");

    //return TWStatus::Set (eTWIterate::STOP, eTWMainStatus::TW_ITERATION_STOPPED);

    return eTWIterate::CONTINUE;
}

eTWIterate
MyListFindFunc (TWBaseElem * /*pElem*/, VPtr /*pParms*/) noexcept
{
        TRACEFUNC;

    TRACE_LOG ("In List Find Function\n");


    return eTWIterate::CONTINUE;
}

class SomeClass : public TWPrimitive {

    public:

};

void
SimpleListTests ()
{
        TRACEFUNC;
     
    TWSList<TUInt64>         newlist;
    TWSList<TUInt64>         secondlist;
    TWSList<VPtr>              ptrlist;
    TWListElem<TUInt64> * intelem1;
    TWListElem<TUInt64> * intelem2;
    TWListElem<TUInt64> * intelem3;
    TWListElem<TUInt64> * intelem;
    TWListElem<TUInt64> * oldelem;
    TWListElem<TUInt64> *    elem;
    TWListElem<VPtr> *         ptrelem;
    bool                         rc;
    TUInt64                      numelems;

    {

            TWSHeldElems    heldelems;

        newlist.InitList ();

        TRACE_LOG ("\n");

        ptrelem = ptrlist.InsertValueHead (nullptr, heldelems);
        CHECKNULL (ptrelem);

        intelem1 = newlist.InsertValueHead (20, heldelems);
        CHECKNULL (intelem1);
        TRACE_LOG ("elem: %d\n", intelem1->GetValue ());

        intelem2 = newlist.InsertValueBefore (21, intelem1, heldelems);
        CHECKNULL (intelem2);
        TRACE_LOG ("elem: %d\n", intelem2->GetValue ());

        intelem3 = newlist.InsertValueAfter (22, intelem2, heldelems);
        CHECKNULL (intelem3);
        TRACE_LOG ("elem: %d\n", intelem3->GetValue ());

        rc = newlist.IterateForward ((TWIterateElemFunc) MyListIterateFunc, nullptr);
        CHECKRC (!rc);

        intelem = newlist.InsertValueTail (23, heldelems, eSyncType::EXCLUSIVE);
        if (!intelem)
            TRACE_STATUS;
        CHECKNULL (intelem);

        rc = newlist.RemoveFromList (intelem, heldelems);
        CHECKRC (!rc);
        intelem->DestroyElem (intelem);

        // NOTE: can only use this AFTER destruction of held objects

        intelem = newlist.TryPopHead ();
        if (intelem)
            intelem->DestroyElem (intelem);
        elem = newlist.PopTail ();
        if (intelem)
            elem->DestroyElem (elem);

        TRACE_LOG ("\n");

        // elem = (decltype(*elem))::CreateElemValue (50);

        // TODO: TUInt64 a = 50;
        // TODO: elem = elem->template CreateElem<> ((CVPtr) &a);

        elem = elem->CreateElemValue<TWListElem<TUInt64>> (50);
        CHECKNULL (elem);
        rc = newlist.InsertElemHead (elem, heldelems);
        CHECKRC (!rc);
        TRACE_LOG ("elem: %d\n", elem->GetValue ());
        oldelem = elem;
        elem    = elem->CreateElemValue<TWListElem<TUInt64>> (51);
        CHECKNULL (elem);
        rc = newlist.InsertElemAfter (elem, oldelem, heldelems);
        CHECKRC (!rc);
        TRACE_LOG ("elem: %d\n", elem->GetValue ());
        oldelem = elem;
        elem    = elem->CreateElemValue<TWListElem<TUInt64>> (52);
        CHECKNULL (elem);
        rc = newlist.InsertElemBefore (elem, oldelem, heldelems);
        CHECKRC (!rc);
        TRACE_LOG ("elem: %d\n", elem->GetValue ());
        elem = elem->CreateElemValue<TWListElem<TUInt64>> (53);
        CHECKNULL (elem);
        TRACE_LOG ("elem: %d\n", elem->GetValue ());
        rc = newlist.InsertElemTail (elem, heldelems, eSyncType::EXCLUSIVE);
        CHECKRC (!rc);
        rc = newlist.RemoveFromList (elem, heldelems);        // TODO: what about held list
        CHECKRC (!rc);
        elem->DestroyElem (elem);

        TRACE_LOG ("\n");

        elem = newlist.FindFirst ((TWIterateElemFunc) MyListFindFunc, nullptr, heldelems);
        elem = newlist.FindNext (elem, (TWIterateElemFunc) MyListFindFunc, nullptr, heldelems);
        elem = newlist.FindPrev (elem, (TWIterateElemFunc) MyListFindFunc, nullptr, heldelems);
        elem = newlist.FindLast ((TWIterateElemFunc) MyListFindFunc, nullptr, heldelems);

        TRACE_LINE;

        elem = newlist.PopHead ();
        if (elem)
            elem->DestroyElem (elem);

        intelem = newlist.PopTail ();
        if (intelem)
            intelem->DestroyElem (intelem);

        TRACE_LINE;
        rc = secondlist.CopyFromList (newlist);
        CHECKRC (!rc);
        TRACE_LINE;
        TRACE_DIAG_ENABLE_ALL;
    }
    {
        TWSHeldElems    heldelems;

        if (rc)
           rc = newlist.DeleteAll ();
        TRACE_DIAG_DISABLE_ALL;
        TRACE_LINE;
        
        CHECKTRUE (newlist.GetNumElems () != 0);
        
        elem = (TWListElem<TUInt64> *) secondlist.GetHead (heldelems);
        CHECKNULL (elem);
        
        TRACE_LOG ("Switch to Next for %p\n", elem);
        elem = (TWListElem<TUInt64> *) secondlist.GetNext (elem, heldelems);
        CHECKNULL (elem);
        
        elem = (TWListElem<TUInt64> *) secondlist.GetPrev (elem, heldelems);
        CHECKNULL (elem);
        
        elem = (TWListElem<TUInt64> *) secondlist.GetTail (heldelems);
        CHECKNULL (elem);
        
        rc = newlist.DeleteAll ();
        CHECKRC (!rc);
        
        elem = (TWListElem<TUInt64> *) newlist.GetHead (heldelems);
        CHECKTRUE (elem != nullptr);
        
        elem = (TWListElem<TUInt64> *) newlist.GetTail (heldelems);
        CHECKTRUE (elem != nullptr);
        
        numelems = secondlist.GetNumElems ();
    }

    TRACE_LOG ("SimpleList ends\n\n");
}

struct tMyClass {

    TUInt64 uSomeVal1;
    TUInt64 uSomeVal2;
};

class MyClass : public TWBaseAllocData {

public:

static  MyClass *   CreateAllocedContent (CVPtr pParms) noexcept;
static  void        DestroyAllocedContent (MyClass * pMyClass) noexcept;

public:

    TUInt64     uMember1;
    TUInt64     uMember2;
};

MyClass *
MyClass::CreateAllocedContent (CVPtr pParms) noexcept
{
        TRACEFUNC;

        MyClass *   elem;

    TRACE_LOG ("In CreateAllocedContent\n");

    elem = (MyClass *) TWMemoryMgr::MemCalloc (sizeof (MyClass));

    if (!elem)
        return nullptr;

    elem->uMember1 = ((tMyClass *) pParms)->uSomeVal1;
    elem->uMember2 = ((tMyClass *) pParms)->uSomeVal2;

    return elem;
}

void
MyClass::DestroyAllocedContent(MyClass* pMyClass) noexcept
{
        TRACEFUNC;

    TRACE_LOG ("In DestroyAllocedContent\n");

    TWMemoryMgr::MemFree (pMyClass, sizeof (MyClass));
}

void
VectorListTests()
{
        TRACEFUNC;

    TWSList<MyClass *> mylist;
    TWSList<MyClass *> mysecondlist;
    tMyClass                    content { 500, 200 };
    TWListElem<MyClass *> * elem;
    TWListElem<MyClass *> * oldelem;
    bool                        rc;

    {

            TWSHeldElems    heldelems;

        elem = mylist.InsertParmsHead (&content, heldelems);
        CHECKNULL (elem);
        TRACE_LOG ("elem: Member1: %d, Member2: %d\n", elem->GetValue ()->uMember1, elem->GetConstValue ()->uMember2);
        oldelem = elem;
        elem    = mylist.InsertParmsAfter (&content, oldelem, heldelems);
        CHECKNULL (elem);
        oldelem = elem;
        elem    = mylist.InsertParmsBefore (&content, oldelem, heldelems);
        CHECKNULL (elem);
        elem = mylist.InsertParmsTail (&content, heldelems, eSyncType::EXCLUSIVE);
        CHECKNULL (elem);
        rc = mylist.DeleteFromList (elem, heldelems);
        CHECKRC (!rc);

        elem = mylist.FindFirst ((TWIterateElemFunc) MyListFindFunc, nullptr, heldelems);
        elem = mylist.FindNext (elem, (TWIterateElemFunc) MyListFindFunc, nullptr, heldelems);
        elem = mylist.FindPrev (elem, (TWIterateElemFunc) MyListFindFunc, nullptr, heldelems);
        elem = mylist.FindLast ((TWIterateElemFunc) MyListFindFunc, nullptr, heldelems);

        if (mysecondlist.CopyFromList (mylist) == false)
            TRACE_LOG ("Copy failed\n");

        TRACE_LOG ("Vector List Iterate\n");

        rc = mylist.IterateForward ((TWIterateElemFunc) MyListIterateFunc, nullptr);
        rc = mysecondlist.IterateBackward ((TWIterateElemFunc) MyListIterateFunc, nullptr);
    }

    rc = mylist.DeleteAll ();
    TRACE_LOG ("VectorList ends\n\n");
}

eTWIterate
MyListIterateFunc (TWListElem<TUInt64> * /*pElem*/, VPtr /*pParms*/) noexcept
{
        TRACEFUNC;
    TRACE_LOG ("In List Iterate Function\n");

    // return TWStatus::Set (eTWIterate::STOP, eTWMainStatus::TW_ITERATION_STOPPED);

        return eTWIterate::CONTINUE;
}

class MyLRUListElem : public TWBaseLRUAllocData {

    public:

static          MyLRUListElem *   CreateAllocedContent   (CVPtr pParms) noexcept;
static          void        DestroyAllocedContent  (MyLRUListElem * pMyLRUListElem) noexcept;

    public:

                TUInt64     uMember1;
                TUInt64     uMember2;

};

class MySecondLRUListElem : public TWBaseLRUListElem {

    public:

static          MySecondLRUListElem * CreateAllocedContent   (CVPtr pParms) noexcept;
static          void            DestroyAllocedContent  (MySecondLRUListElem * pMyLRUListElem) noexcept;

    public:

                TUInt64         uMember1;
                TUInt64         uMember2;
};

MyLRUListElem *
MyLRUListElem::CreateAllocedContent (CVPtr pParms) noexcept
{
        TRACEFUNC;

        MyLRUListElem *   elem;

    elem = (MyLRUListElem *) TWMemoryMgr::MemCalloc (sizeof (MyLRUListElem));

    if (!elem)
        return nullptr;

    TRACE_LOG ("In CreateAllocedContent of LRUListElem: %p\n", elem);

    elem->uMember1 = ((tMyClass *) pParms)->uSomeVal1;
    elem->uMember2 = ((tMyClass *) pParms)->uSomeVal2;

    return elem;
}

void
MyLRUListElem::DestroyAllocedContent (MyLRUListElem * pMyLRUListElem) noexcept
{
        TRACEFUNC;

    TRACE_LOG ("In DestroyAllocedContent: %p\n", pMyLRUListElem);

    TWMemoryMgr::MemFree (pMyLRUListElem, sizeof (MyLRUListElem));
}

MySecondLRUListElem *
MySecondLRUListElem::CreateAllocedContent (CVPtr pParms) noexcept
{
        TRACEFUNC;

        MySecondLRUListElem * elem;

    TRACE_LOG ("In CreateAllocedContent\n");

    elem = (MySecondLRUListElem *) TWMemoryMgr::MemCalloc (sizeof (MySecondLRUListElem));

    if (!elem)
        return nullptr;

    elem->uMember1 = ((tMyClass *) pParms)->uSomeVal1;
    elem->uMember2 = ((tMyClass *) pParms)->uSomeVal2;

    return elem;
}

void
MySecondLRUListElem::DestroyAllocedContent (MySecondLRUListElem * pMySecondLRUListElem) noexcept
{
        TRACEFUNC;

    TRACE_LOG ("In DestroyAllocedContent\n");

    TWMemoryMgr::MemFree (pMySecondLRUListElem, sizeof (MySecondLRUListElem));
}

void
LRUListTests ()
{
        TRACEFUNC;
        TWLRUList<MyLRUListElem *> * mylist;
        TWLRUList<MyLRUListElem *> * mysecondlist;
        tMyClass          content{500, 200};
        TWLRUListElem<MyLRUListElem *> * elem;
        TWLRUListElem<MyLRUListElem *> * oldelem;
        bool    rc;
        TWLRUCache  lrucache;

    {

            TWSHeldElems    heldelems;

        TRACE_LOG ("LRUListTests Start\n");

        rc = lrucache.InitLRUCache (TW_LRUCACHE_DEFAULT_NUMROOTS, TW_LRUCACHE_DEFAULT_RECLAIM_THRESHOLD);
        CHECKRC (!rc);

        mylist = TWLRUList<MyLRUListElem *>::CreateLRUList (&lrucache);
        CHECKNULL (mylist);
        mysecondlist = TWLRUList<MyLRUListElem *>::CreateLRUList (&lrucache);
        CHECKNULL (mysecondlist);

        elem = nullptr;
        TRACE_LOG ("sForLRU :%d, %d\n", IsForLRU<MyLRUListElem *> (), IsForLRU<MyLRUListElem> ());

        elem = mylist->InsertParmsHead (&content, heldelems);
        CHECKNULL (elem);
        oldelem = elem;

        TRACE_LOG ("Inserted %p after Head\n", oldelem);
        elem = mylist->InsertParmsBefore (&content, oldelem, heldelems);
        CHECKNULL (elem);
        TRACE_LOG ("\n");
        oldelem = elem;
        TRACE_LOG ("\n");
        elem = mylist->InsertParmsAfter (&content, oldelem, heldelems);
        CHECKNULL (elem);
        TRACE_LOG ("\n");
        TRACE_LOG ("\n");
        elem = mylist->InsertParmsTail (&content, heldelems, eSyncType::EXCLUSIVE, eTWLRUPriority::LOW);
        CHECKNULL (elem);
        rc = mylist->DeleteFromList (elem, heldelems);
        CHECKRC (!rc);
    }
    {
        TWSHeldElems    heldelems;
    
        rc = mylist->DeleteAll ();
        CHECKRC (!rc);

        elem = mylist->FindFirst ((TWIterateElemFunc) MyListFindFunc, nullptr, heldelems);
        elem = mylist->FindNext (elem, (TWIterateElemFunc) MyListFindFunc, nullptr, heldelems);
        elem = mylist->FindPrev (elem, (TWIterateElemFunc) MyListFindFunc, nullptr, heldelems);
        elem = mylist->FindLast ((TWIterateElemFunc) MyListFindFunc, nullptr, heldelems);

        TRACE_LOG ("\n");

        {
            TRACE_LOG ("GetHead()\n");
            elem = mylist->GetHead (heldelems);

            if (!elem)
                TRACE_LOG ("No Head\n");
            else {

                TRACE_LOG ("GetNext()\n");
                elem = mylist->GetNext (elem, heldelems);
            }

            if (!elem)
                TRACE_LOG ("No Head\n");
            else {

                TRACE_LOG ("GetPrev()\n");
                elem = mylist->GetPrev (elem, heldelems);
            }

            if (!elem)
                TRACE_LOG ("No Head\n");
            else {

                TRACE_LOG ("GetTail()\n");
                elem = mylist->GetTail (heldelems);
            }

            if (!elem)
                TRACE_LOG ("No Head\n");

            rc = (mylist->GetNumElems () != 0);

            rc = mylist->IterateForward ((TWIterateElemFunc) MyListIterateFunc, nullptr);

            rc = mylist->IterateBackward ((TWIterateElemFunc) MyListIterateFunc, nullptr);
        }

        TRACE_LOG ("\n");

        elem = elem->CreateElem<TWLRUListElem<MyLRUListElem *>> (&content);
        CHECKNULL (elem);
        rc = mylist->InsertElemHead (elem, heldelems);
        CHECKRC (!rc);
        oldelem = elem;

        TRACE_LOG ("\n");

        elem = elem->CreateElem<TWLRUListElem<MyLRUListElem *>> (&content);
        CHECKNULL (elem);
        rc = mylist->InsertElemBefore (elem, oldelem, heldelems);
        CHECKRC (!rc);
        oldelem = elem;

        TRACE_LOG ("\n");

        elem = elem->CreateElem<TWLRUListElem<MyLRUListElem *>> (&content);
        CHECKNULL (elem);
        rc = mylist->InsertElemAfter (elem, oldelem, heldelems);
        CHECKRC (!rc);

        TRACE_LOG ("\n");

        elem = elem->CreateElem<TWLRUListElem<MyLRUListElem *>> (&content);
        CHECKNULL (elem);
        rc = mylist->InsertElemTail (elem, heldelems, eSyncType::EXCLUSIVE);
        CHECKRC (!rc);

        TRACE_LOG ("\n");

        rc = mylist->RemoveFromList (elem, heldelems);        // the tail which we just had
        TRACE_LINE;
        CHECKRC (!rc);
        elem->ReleaseLock (heldelems);

        rc = mylist->InsertElemHead (elem, heldelems);
        CHECKRC (!rc);

        TRACE_LOG ("\n");

        TRACE_LINE;
        elem = mylist->PopHead ();
        TRACE_LINE;

        if (elem) {

            TRACE_LINE;
            elem->ReleaseLock (heldelems);
            rc = mylist->InsertElemHead (elem, heldelems);
            CHECKRC (!rc);
        }

        TRACE_LINE;
        elem = mylist->PopTail ();

        if (elem) {

            TRACE_LINE;
            elem->ReleaseLock (heldelems);
            rc = mylist->InsertElemHead (elem, heldelems);
            CHECKRC (!rc);
            TRACE_LINE;
        }

        rc = mysecondlist->CopyFromList (*mylist);
        CHECKRC (!rc);

        TRACE_LOG ("\n");

        elem = mysecondlist->GetHead (heldelems);
        CHECKNULL (elem);
        elem = mysecondlist->GetNext (elem, heldelems);
        CHECKNULL (elem);
        elem = mysecondlist->GetPrev (elem, heldelems);
        CHECKNULL (elem);
        elem = mysecondlist->GetTail (heldelems);
        CHECKNULL (elem);

        TRACE_LOG ("\n");
    }

    TWLRUList<MyLRUListElem *>::DestroyLRUList (mysecondlist);
    
    TWLRUList<MyLRUListElem *>::DestroyLRUList (mylist);

    lrucache.Finalize ();
}

void
RunListTests()
{
        TRACEFUNC;
    
    //SimpleListTests ();
    VectorListTests ();
    //LRUListTests ();
}
