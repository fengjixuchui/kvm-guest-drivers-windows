#pragma once
#include "ParaNdis-VirtQueue.h"
#include "ParaNdis-AbstractPath.h"

class CParaNdisRX : public CParaNdisTemplatePath<CVirtQueue>, public CNdisAllocatable<CParaNdisRX, 'XRHR'>
{
  public:
    CParaNdisRX();
    ~CParaNdisRX();

    bool Create(PPARANDIS_ADAPTER Context, UINT DeviceQueueIndex);

    BOOLEAN AddRxBufferToQueue(pRxNetDescriptor pBufferDescriptor);

    void PopulateQueue();

    void FreeRxDescriptorsFromList();

    void ReuseReceiveBuffer(pRxNetDescriptor pBuffersDescriptor)
    {
        TPassiveSpinLocker autoLock(m_Lock);

        ReuseReceiveBufferNoLock(pBuffersDescriptor);
    }

    BOOLEAN IsRxBuffersShortage()
    {
        return m_NetNofReceiveBuffers < m_MinRxBufferLimit;
    }

    VOID ProcessRxRing(CCHAR nCurrCpuReceiveQueue);

    BOOLEAN RestartQueue();

    void Shutdown()
    {
        TPassiveSpinLocker autoLock(m_Lock);

        m_VirtQueue.Shutdown();
        m_Reinsert = false;
    }

    void KickRXRing();

    PARANDIS_RECEIVE_QUEUE &UnclassifiedPacketsQueue()
    {
        return m_UnclassifiedPacketsQueue;
    }
    UINT GetFreeRxBuffers() const
    {
        return m_NetNofReceiveBuffers;
    }
    BOOLEAN AllocateMore();

  private:
    /* list of Rx buffers available for data (under VIRTIO management) */
    LIST_ENTRY m_NetReceiveBuffers;
    UINT m_NetNofReceiveBuffers = 0;
    UINT m_NetMaxReceiveBuffers = 0;
    UINT m_MinRxBufferLimit;

    UINT m_nReusedRxBuffersCounter = 0;
    UINT m_nReusedRxBuffersLimit = 0;

    bool m_Reinsert = true;

    PARANDIS_RECEIVE_QUEUE m_UnclassifiedPacketsQueue;

    void ReuseReceiveBufferNoLock(pRxNetDescriptor pBuffersDescriptor);

  private:
    int PrepareReceiveBuffers();
    pRxNetDescriptor CreateRxDescriptorOnInit();
    void RecalculateLimits();
};

#ifdef PARANDIS_SUPPORT_RSS
VOID ParaNdis_ResetRxClassification(PARANDIS_ADAPTER *pContext);
#endif
