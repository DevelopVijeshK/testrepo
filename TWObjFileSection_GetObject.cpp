// Sri Ganeshji : Sri Balaji : Sri Pitreshwarji : Sri Durgaji : Sri Venkateshwara

#include    "TWObjFileSection.hpp"

bool
TWObjFileSection::GetObject (TUInt64 pVersion, TWDiskObjID & pDiskObjID, TWSObject * pObjFromDisk, TWIntList * pVersionList, TUInt64 * pSubStoreID, eTWObjStatus * pObjStatus)
{
        TRACEFUNC;

        TWObjDataBlock *    objblock;
        TUInt64             blocknum;
        TUInt64             ndxid;
        TUInt64             reclaimversion;
        TWStackHeldBlocks   heldlist;                   // which we need to 'release' when done

    // when we work, we don't wont our vFileData to be unstable.  We just need 'two values' from here

    {
            StackLock   slock (&vSyncFlag, eSyncType::SHARED, vObjFile->GetReadTimeout ());

        if (!slock.IsLocked ())
            return TWStatus::Set (false, TW_FILE_BUSY);

        if (pDiskObjID.uSecondaryStoreID)
            blocknum = vSectionHeaderBlock->vSecondaryStoreBlock;
        else
            blocknum = vSectionHeaderBlock->vPrimaryStoreBlock;

        reclaimversion = vSectionHeaderBlock->vReclaimedVersion;
    }

    if (!vObjFile->GetLeafBlock (pDiskObjID, blocknum, (TWObjBlock **) &objblock, &ndxid, &heldlist))
        return false;

    while (objblock->uThisBlockVersion > pVersion) {

        if (!pVersionList->AtomicAddInt64ToTail (objblock->uThisBlockVersion))
            return false;

        if (!vObjFile->GetPrevObjBlock ((TWObjBlock **) &objblock, &blocknum, reclaimversion, &heldlist)) {

            if (TWStatus::GetMainStatus () == TW_NOTFOUND)
                return TWStatus::Set (false, TW_NOTFOUND, TW_VERSION_NOTFOUND);

            return false;
        }
    }

        // insert our version into the list

    if (!pVersionList->AtomicAddInt64ToTail (objblock->uThisBlockVersion))
        return false;

    return InternalGetObjectFromBlock (objblock, ndxid, pObjFromDisk, pSubStoreID, pObjStatus, &heldlist);
}

bool
TWObjFileSection::InternalGetObjectFromBlock (TWObjDataBlock * pObjBlock, TUInt64 pNdxID, TWSObject * pObjFromDisk, TUInt64 * pSubStoreID, eTWObjStatus * pObjStatus, TWStackHeldBlocks * pHeldList)
{
        TRACEFUNC;

        bool                rc{true};
        TUInt64             buflen;
        TUInt32             srclen;
        TUInt32             lentocopy;   
        TUInt64             spilloverid;
        TUInt8 *            srcbuf;
        TUInt8 *            destbuf;
        TWDiskObj *         diskobj;

    // we have a valid block! if our packing is 1, this is it.
    // else - we need to check

    srcbuf      = (TUInt8 *) pObjBlock->uDiskObjs;

    if (pNdxID) {

        CHECKTRUE (pObjBlock->uBlockID.uObjPacking == 1);

        buflen = vObjFile->GetBlockContentSize();

        buflen /= pObjBlock->uBlockID.uObjPacking;

        srcbuf += (buflen * pNdxID);
    }

    diskobj = (TWDiskObj *) srcbuf;

    if (pObjStatus)
        *pObjStatus = (eTWObjStatus) diskobj->uObjStatus;

    if ((diskobj->uObjStatus != (TUInt32)eTWObjStatus::NOT_CREATED) &&
        (diskobj->uObjStatus != (TUInt32)eTWObjStatus::EXISTS) &&
        (diskobj->uObjStatus != (TUInt32)eTWObjStatus::EMPTY))
        return rc;

    if (pSubStoreID)
        *pSubStoreID = diskobj->uSecondaryStoreID;

    if (diskobj->uObjStatus == (TUInt32) eTWObjStatus::NOT_CREATED)
        return TWStatus::Set (false, TW_NOTFOUND);

    if (diskobj->uObjStatus == (TUInt32)eTWObjStatus::EMPTY)
        return rc;

    CHECKTRUE (diskobj->uObjStatus != (TUInt32)eTWObjStatus::EXISTS);
    CHECKTRUE (!diskobj->uTotalContentLength);

    if (!diskobj->uTotalContentLength)
        return TWStatus::Set (false, TW_NOTFOUND, TW_EMPTY);

    // we have something we can use to construct!

    lentocopy   = diskobj->uTotalContentLength;
    srclen      = diskobj->uDiskObjLength;

    CHECKTRUE (srclen > lentocopy);

    destbuf = nullptr;                          // stop compiler from complaining - since it does not recognize 'rc.IsSuccess' validation before we reuse destbuf

    if (!pObjFromDisk->GetImagePtr (lentocopy, &destbuf))
        return false;

    srcbuf      = diskobj->uDiskObjContent;

    if (pObjBlock->uBlockID.uObjPacking > 1)    // need to skip over the version
        srcbuf += sizeof (TUInt64);

    if (srclen < lentocopy) {

        spilloverid = ((TUInt64 *) srcbuf)[0];

        srcbuf += sizeof (TUInt64);

    } else
        spilloverid = 0;

    TWMemoryMgr::MemCpy (destbuf, srcbuf, srclen);

    lentocopy   -= srclen;
    destbuf     += srclen;                      // move our pointer forward

    if (lentocopy) {

        CHECKTRUE (spilloverid == 0);

        rc = InternalGetFromSpillOver (destbuf, lentocopy, spilloverid, pHeldList);
    }

    return rc;
}

bool
TWObjFileSection::InternalGetFromSpillOver (TUInt8 * pDestBuf, TUInt32 pLenToCopy, TUInt64 pSpillOverID, TWStackHeldBlocks * pHeldList)
{
        TRACEFUNC;

        bool                    rc{true};
        TUInt32                 i;
        TUInt64                 blocknum;
        TWSpillOverBlock *      spilloverblock;
        TWSpillOverObj *        spilloverobj;
        TUInt8 *                destbuf;
        TUInt8 *                srcbuf;
        TUInt64                 dbobjndx;
        TUInt64                 spilloverlen;
        TUInt32                 srclen;
        TUInt32                 lentocopy;   
        TWDiskObjID             spilloverobjid;

    for (i = TWOBJFILE_MAX_SPILLOVER_STORES - 1; i != 0; i--) {

        if (vObjFile->vSpillOverStoreLen[i] >= (pLenToCopy - sizeof(TWSpillOverObj)))
            break;
    }

    spilloverlen = vObjFile->vSpillOverStoreLen[i];

    {
            StackMustLock   slock (&vSyncFlag, eSyncType::SHARED);

        blocknum  = vSectionHeaderBlock->vSpillOverStoreBlock[i];
    }

    CHECKTRUE (!blocknum);

    spilloverobjid.Init (0, i, pSpillOverID);

    rc = vObjFile->GetLeafBlock (spilloverobjid, blocknum, (TWObjBlock **) &spilloverblock, &dbobjndx, pHeldList);
    
    CHECKTRUE ((!rc) && (TWStatus::GetMainStatus() == TW_NOTFOUND));

    if (!rc)
        return rc;

        // we have a valid block! if our packing is 1, this is it.
        // else - we need to check

    CHECKTRUE (dbobjndx != pSpillOverID % spilloverblock->uBlockID.uObjPacking);

    srcbuf      = (TUInt8 *) spilloverblock->uSpillOverObjs;

    if ((spilloverblock->uBlockID.uObjPacking > 1) &&
        (dbobjndx > 0))
        srcbuf += (spilloverlen * dbobjndx);

    spilloverobj = (TWSpillOverObj *) srcbuf;

        // well - we have our diskobj!  lets get to work!

    lentocopy               = spilloverobj->uTotalContentLength;
    srclen                  = spilloverobj->uSpillOverLength;

    CHECKTRUE (srclen > lentocopy);

    if (srclen > lentocopy)
        return TWStatus::Set (false, TW_PANIC_POSSIBLE_BUG);

    srcbuf                  = spilloverobj->uSpillOverContent;
    destbuf                 = pDestBuf;

    if (srclen < lentocopy) {

        pSpillOverID = ((TUInt64 *) srcbuf)[0];

        srcbuf += sizeof (TUInt64);

    } else
        pSpillOverID = 0;

    TWMemoryMgr::MemCpy (destbuf, srcbuf, srclen);

    lentocopy   -= srclen;
    destbuf     += srclen;                      // move our pointer forward

    if (lentocopy) {

        CHECKTRUE (pSpillOverID == 0);

        rc = InternalGetFromSpillOver (destbuf, lentocopy, pSpillOverID, pHeldList);
    }

    return rc;                              // we are DONE!
}





