#ifndef CCOLLECTIONLOADER_H
#define CCOLLECTIONLOADER_H

/*

THIS CLASS IS WRITTEN BY ERIK RUFELT FOR THE PURPOSE OF HANDLING CUSTOM FONTS WITH
FW1FONTWRAPPER. ALL CREDITS BELONG TO HIM. EXACT CODE DETAILS: http://fw1.codeplex.com/discussions/445629

*/

#include "CFontEnum.h"
#include <string>

//dll
#include "FW/FW1FontWrapper.h"
//Lib
#pragma comment(lib, "FW1FontWrapper.lib")

class CCollectionLoader : public IDWriteFontCollectionLoader {
    public:
        virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) {
            if(ppvObject == NULL)
                return E_INVALIDARG;
            
            if(IsEqualIID(riid, __uuidof(IUnknown))) {
                *ppvObject = static_cast<IUnknown*>(this);
                AddRef();
                return S_OK;
            }
            else if(IsEqualIID(riid, __uuidof(IDWriteFontCollectionLoader))) {
                *ppvObject = static_cast<IDWriteFontCollectionLoader*>(this);
                AddRef();
                return S_OK;
            }
            
            *ppvObject = NULL;
            return E_NOINTERFACE;
        }
        
        virtual ULONG STDMETHODCALLTYPE AddRef() {
            return static_cast<ULONG>(InterlockedIncrement(reinterpret_cast<LONG*>(&m_cRefCount)));
        }
        
        virtual ULONG STDMETHODCALLTYPE Release() {
            ULONG newCount = static_cast<ULONG>(InterlockedDecrement(reinterpret_cast<LONG*>(&m_cRefCount)));
            
            if(newCount == 0)
                delete this;
            
            return newCount;
        }
        
        virtual HRESULT STDMETHODCALLTYPE CreateEnumeratorFromKey(
            IDWriteFactory *pFactory,
            const void *collectionKey,
            UINT32 collectionKeySize,
            IDWriteFontFileEnumerator **ppFontFileEnumerator
        ) {
            collectionKey;
            collectionKeySize;
            
            if(pFactory == NULL || ppFontFileEnumerator == NULL)
                return E_INVALIDARG;
            
            IDWriteFontFile *pFontFile;
            HRESULT hResult = pFactory->CreateFontFileReference(m_filepath.c_str(), NULL, &pFontFile);
            if(FAILED(hResult))
                return hResult;
            
            CFontEnum *pEnum = new CFontEnum(pFontFile);
            *ppFontFileEnumerator = pEnum;
            
            pFontFile->Release();
            
            return S_OK;
        }
    
    public:
        CCollectionLoader(const WCHAR *szFontfilePath) :
            m_cRefCount(1),
            m_filepath(szFontfilePath)
        {
        }
    
    private:
        ~CCollectionLoader() {
        }
    
    private:
        ULONG                       m_cRefCount;
        std::wstring                m_filepath;
    
    private:
        CCollectionLoader(const CCollectionLoader&);
        CCollectionLoader& operator=(const CCollectionLoader&);
};

#endif