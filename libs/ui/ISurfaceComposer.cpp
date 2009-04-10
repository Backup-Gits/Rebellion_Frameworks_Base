/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// tag as surfaceflinger
#define LOG_TAG "SurfaceFlinger"

#include <stdint.h>
#include <sys/types.h>

#include <utils/Parcel.h>
#include <utils/IMemory.h>
#include <utils/IPCThreadState.h>
#include <utils/IServiceManager.h>

#include <ui/ISurfaceComposer.h>
#include <ui/DisplayInfo.h>

// ---------------------------------------------------------------------------

#define LIKELY( exp )       (__builtin_expect( (exp) != 0, true  ))
#define UNLIKELY( exp )     (__builtin_expect( (exp) != 0, false ))

// ---------------------------------------------------------------------------

namespace android {

class BpSurfaceComposer : public BpInterface<ISurfaceComposer>
{
public:
    BpSurfaceComposer(const sp<IBinder>& impl)
        : BpInterface<ISurfaceComposer>(impl)
    {
    }

    virtual sp<ISurfaceFlingerClient> createConnection()
    {
        uint32_t n;
        Parcel data, reply;
        data.writeInterfaceToken(ISurfaceComposer::getInterfaceDescriptor());
        remote()->transact(BnSurfaceComposer::CREATE_CONNECTION, data, &reply);
        return interface_cast<ISurfaceFlingerClient>(reply.readStrongBinder());
    }

    virtual sp<IMemory> getCblk() const
    {
        Parcel data, reply;
        data.writeInterfaceToken(ISurfaceComposer::getInterfaceDescriptor());
        remote()->transact(BnSurfaceComposer::GET_CBLK, data, &reply);
        return interface_cast<IMemory>(reply.readStrongBinder());
    }

    virtual void openGlobalTransaction()
    {
        Parcel data, reply;
        data.writeInterfaceToken(ISurfaceComposer::getInterfaceDescriptor());
        remote()->transact(BnSurfaceComposer::OPEN_GLOBAL_TRANSACTION, data, &reply);
    }

    virtual void closeGlobalTransaction()
    {
        Parcel data, reply;
        data.writeInterfaceToken(ISurfaceComposer::getInterfaceDescriptor());
        remote()->transact(BnSurfaceComposer::CLOSE_GLOBAL_TRANSACTION, data, &reply);
    }

    virtual status_t freezeDisplay(DisplayID dpy, uint32_t flags)
    {
        Parcel data, reply;
        data.writeInterfaceToken(ISurfaceComposer::getInterfaceDescriptor());
        data.writeInt32(dpy);
        data.writeInt32(flags);
        remote()->transact(BnSurfaceComposer::FREEZE_DISPLAY, data, &reply);
        return reply.readInt32();
    }

    virtual status_t unfreezeDisplay(DisplayID dpy, uint32_t flags)
    {
        Parcel data, reply;
        data.writeInterfaceToken(ISurfaceComposer::getInterfaceDescriptor());
        data.writeInt32(dpy);
        data.writeInt32(flags);
        remote()->transact(BnSurfaceComposer::UNFREEZE_DISPLAY, data, &reply);
        return reply.readInt32();
    }

    virtual int setOrientation(DisplayID dpy, int orientation, uint32_t flags)
    {
        Parcel data, reply;
        data.writeInterfaceToken(ISurfaceComposer::getInterfaceDescriptor());
        data.writeInt32(dpy);
        data.writeInt32(orientation);
        data.writeInt32(flags);
        remote()->transact(BnSurfaceComposer::SET_ORIENTATION, data, &reply);
        return reply.readInt32();
    }

    virtual void bootFinished()
    {
        Parcel data, reply;
        data.writeInterfaceToken(ISurfaceComposer::getInterfaceDescriptor());
        remote()->transact(BnSurfaceComposer::BOOT_FINISHED, data, &reply);
    }

    virtual void signal() const
    {
        Parcel data, reply;
        data.writeInterfaceToken(ISurfaceComposer::getInterfaceDescriptor());
        remote()->transact(BnSurfaceComposer::SIGNAL, data, &reply, IBinder::FLAG_ONEWAY);
    }
};

IMPLEMENT_META_INTERFACE(SurfaceComposer, "android.ui.ISurfaceComposer");

// ----------------------------------------------------------------------

#define CHECK_INTERFACE(interface, data, reply) \
        do { if (!data.enforceInterface(interface::getInterfaceDescriptor())) { \
            LOGW("Call incorrectly routed to " #interface); \
            return PERMISSION_DENIED; \
        } } while (0)

status_t BnSurfaceComposer::onTransact(
    uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
    status_t err = BnInterface<ISurfaceComposer>::onTransact(code, data, reply, flags);
    if (err == NO_ERROR)
        return err;

    CHECK_INTERFACE(ISurfaceComposer, data, reply);

    switch(code) {
        case CREATE_CONNECTION: {
            sp<IBinder> b = createConnection()->asBinder();
            reply->writeStrongBinder(b);
        } break;
        case OPEN_GLOBAL_TRANSACTION: {
            openGlobalTransaction();
        } break;
        case CLOSE_GLOBAL_TRANSACTION: {
            closeGlobalTransaction();
        } break;
        case SET_ORIENTATION: {
            DisplayID dpy = data.readInt32();
            int orientation = data.readInt32();
            uint32_t flags = data.readInt32();
            reply->writeInt32( setOrientation(dpy, orientation, flags) );
        } break;
        case FREEZE_DISPLAY: {
            DisplayID dpy = data.readInt32();
            uint32_t flags = data.readInt32();
            reply->writeInt32( freezeDisplay(dpy, flags) );
        } break;
        case UNFREEZE_DISPLAY: {
            DisplayID dpy = data.readInt32();
            uint32_t flags = data.readInt32();
            reply->writeInt32( unfreezeDisplay(dpy, flags) );
        } break;
        case BOOT_FINISHED: {
            bootFinished();
        } break;
        case SIGNAL: {
            signal();
        } break;
        case GET_CBLK: {
            sp<IBinder> b = getCblk()->asBinder();
            reply->writeStrongBinder(b);
        } break;
        default:
            return UNKNOWN_TRANSACTION;
    }
    return NO_ERROR;
}

// ----------------------------------------------------------------------------

};
