// implemented in Liberty https://github.com/HaydnTrigg/Liberty

// system.dll - the DACOM "SystemContainer" component.
//
// Decompilation ported from the Liberty reference source
// (A:\Liberty\Code\System\{SysContainer,DllMain}.cpp + DACOM headers), adapted
// to compile under MSVC 6.0 (the original toolchain). The Liberty headers target
// a modern compiler (C++20: stdint.h, constexpr, variadic macros) and cannot be
// included here, so the minimal DACOM framework they describe is reproduced
// inline. Class/template structure, member order and virtual-method order mirror
// the originals so VC6 regenerates the same vtables, calling conventions and
// mangled names.

#include <windows.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include <malloc.h>

#include "FDump.h"
#include "TempStr.h"

// NOTE: the clearing behavior of the new operator is necessary
// NOTE: for some components, hence let it in there.
//
#define DA_HEAP_DEFINE_NEW_OPERATOR(classname)                             \
    void *classname::operator new(size_t size) { return calloc(size, 1); } \
    void classname::operator delete(void *ptr) { free(ptr); }

//==========================================================================//
// Minimal DACOM framework (VC6 port of Core/typedefs.h + DACOM/*.h)
//==========================================================================//

enum GENRESULT
{
    GR_OK = 0,
    GR_GENERIC = -1,
    GR_INVALID_PARMS = -2,
    GR_INTERFACE_UNSUPPORTED = -3,
    GR_OUT_OF_MEMORY = -4,
    GR_OUT_OF_SPACE = -5,
    GR_FILE_ERROR = -6,
    GR_NOT_IMPLEMENTED = -7,
    GR_DATA_NOT_FOUND = -8
};

#define DACOM_API __stdcall
#define DACOM_DEFMETHOD(method) virtual GENRESULT DACOM_API method
#define DACOM_DEFMETHOD_(type, method) virtual type DACOM_API method
#define DACOM_NO_VTABLE __declspec(novtable)
#define DACOM_LOW_PRIORITY 0x40000000

// IIDs are the versioned interface strings ("1.11_<name>").
#define IID_IDAComponent "1.11_IDAComponent"
#define IID_IComponentFactory "1.11_IComponentFactory"
#define IID_IAggregateComponent "1.11_IAggregateComponent"
#define IID_ISystemComponent "1.11_ISystemComponent"
#define IID_ISystemContainer "1.11_ISystemContainer"
#define IID_IDAConnectionPointContainer "1.11_IDAConnectionPointContainer"
#define IID_IProfileParser "1.11_IProfileParser"
#define IID_ICOManager "1.11_ICOManager"

struct IDAComponent;

struct DACOMDESC
{
    U32 size;
    const C8 *interface_name;
    DACOMDESC(const C8 *_interface_name = 0)
    {
        size = sizeof(*this);
        interface_name = _interface_name;
    }
};

struct AGGDESC : public DACOMDESC
{
    struct IDAComponent *outer;
    struct IDAComponent **inner;
    const C8 *description;
    AGGDESC(const C8 *_interfaceName = 0, const C8 *_description = 0) : DACOMDESC(_interfaceName)
    {
        outer = 0;
        inner = 0;
        description = _description;
        size = sizeof(*this);
    }
};

struct DACOM_NO_VTABLE IDAComponent
{
    DACOM_DEFMETHOD(QueryInterface)(const C8 *interface_name, void **instance) = 0;
    DACOM_DEFMETHOD_(U32, AddRef)(void) = 0;
    DACOM_DEFMETHOD_(U32, Release)(void) = 0;
};

struct DACOM_NO_VTABLE IComponentFactory : public IDAComponent
{
    DACOM_DEFMETHOD(CreateInstance)(DACOMDESC *descriptor, void **instance) = 0;
};

struct DACOM_NO_VTABLE IAggregateComponent : public IDAComponent
{
    DACOM_DEFMETHOD(Initialize)(void) = 0;
};

struct DACOM_NO_VTABLE ISystemComponent : public IAggregateComponent
{
    DACOM_DEFMETHOD_(void, Update)(void) = 0;
};

struct DACOM_NO_VTABLE ISystemContainer : public ISystemComponent
{
    DACOM_DEFMETHOD(LoadSystemComponents)(void) = 0;
    DACOM_DEFMETHOD(Shutdown)(void) = 0;
    DACOM_DEFMETHOD(AddComponent)(const AGGDESC *descriptor) = 0;
};

struct IDAConnectionPoint;
typedef BOOL32(__stdcall *CONNCONTAINER_ENUM_PROC)(struct IDAConnectionPointContainer *container, struct IDAConnectionPoint *connPoint, void *context);

struct DACOM_NO_VTABLE IDAConnectionPointContainer : public IDAComponent
{
    DACOM_DEFMETHOD(FindConnectionPoint)(const C8 *connectionName, struct IDAConnectionPoint **connPoint) = 0;
    DACOM_DEFMETHOD_(BOOL32, EnumerateConnectionPoints)(CONNCONTAINER_ENUM_PROC proc, void *context) = 0;
};

struct DACOM_NO_VTABLE IProfileParser : public IDAComponent
{
    DACOM_DEFMETHOD(Initialize)(const C8 *fileName, int access) = 0;
    DACOM_DEFMETHOD_(BOOL32, EnumerateSections)(void *proc, void *context) = 0;
    DACOM_DEFMETHOD_(HANDLE, CreateSection)(const C8 *sectionName, int mode) = 0;
    DACOM_DEFMETHOD_(BOOL32, CloseSection)(HANDLE hSection) = 0;
    DACOM_DEFMETHOD_(U32, ReadProfileLine)(HANDLE hSection, U32 lineNumber, C8 *buffer, U32 bufferSize) = 0;
    DACOM_DEFMETHOD_(U32, ReadKeyValue)(HANDLE hSection, const C8 *keyName, C8 *buffer, U32 bufferSize) = 0;
};

struct DACOM_NO_VTABLE ICOManager : public IComponentFactory
{
    DACOM_DEFMETHOD(RegisterComponent)(IComponentFactory *component, const C8 *interface_name, U32 priority) = 0;
    DACOM_DEFMETHOD(UnregisterComponent)(IComponentFactory *component, const C8 *interface_name) = 0;
    DACOM_DEFMETHOD(EnumerateComponents)(const C8 *interface_name, void *callback, void *context) = 0;
    DACOM_DEFMETHOD(AddLibrary)(const C8 *DLL_filename) = 0;
    DACOM_DEFMETHOD(RemoveLibrary)(const C8 *DLL_filename) = 0;
    DACOM_DEFMETHOD(ShutDown)(void) = 0;
    DACOM_DEFMETHOD(SetINIConfig)(const C8 *info, U32 flags) = 0;
};

extern "C"
{
    __declspec(dllimport) ICOManager *DACOM_Acquire(void);
}

// ---- DACOM inbound interface map (TComponent.h) ----
struct _DACOM_INTMAP_ENTRY
{
    const C8 *interface_name;
    U32 offset;
};

#define daoffsetofclass(base, derived) ((U32)(static_cast<base *>((derived *)8)) - 8)

#define BEGIN_DACOM_MAP_INBOUND(x)                              \
public:                                                         \
    const static _DACOM_INTMAP_ENTRY *__stdcall _GetEntriesIn() \
    {                                                           \
        typedef x _DaComMapClass;                               \
        static const _DACOM_INTMAP_ENTRY _entries[] = {
#define DACOM_INTERFACE_ENTRY2(x, y) {x, daoffsetofclass(y, _DaComMapClass)},
#define END_DACOM_MAP() { 0, 0 } } \
    ;                              \
    return _entries;               \
    }

// ---- DAComponent / DAComponentInner templates (TComponent.h) ----
template <class Base>
struct DAComponent : public Base
{
    U32 ref_count;
    DAComponent(void) { ref_count = 1; }
    DACOM_DEFMETHOD(QueryInterface)(const C8 *interface_name, void **instance);
    DACOM_DEFMETHOD_(U32, AddRef)(void);
    DACOM_DEFMETHOD_(U32, Release)(void);
};

template <class Base>
GENRESULT DAComponent<Base>::QueryInterface(const C8 *interface_name, void **instance)
{
    int i;
    const _DACOM_INTMAP_ENTRY *interfaces = Base::_GetEntriesIn();
    for (i = 0; interfaces[i].interface_name; i++)
    {
        if (strcmp(interfaces[i].interface_name, interface_name) == 0)
        {
            IDAComponent *result;
            if (interfaces[i].offset & 0x80000000)
                result = *((IDAComponent **)(((char *)this) + (interfaces[i].offset & ~0x80000000)));
            else
                result = (IDAComponent *)(((char *)this) + interfaces[i].offset);
            result->AddRef();
            *instance = result;
            return GR_OK;
        }
    }
    *instance = 0;
    return GR_INTERFACE_UNSUPPORTED;
}

template <class Base>
U32 DAComponent<Base>::AddRef(void)
{
    ref_count++;
    return ref_count;
}

template <class Base>
U32 DAComponent<Base>::Release(void)
{
    if (ref_count > 0)
        ref_count--;
    if (ref_count == 0)
    {
        ref_count++;
        delete this;
        return 0;
    }
    return ref_count;
}

template <class Type>
struct DAComponentInner : public IDAComponent
{
    U32 ref_count;
    Type *owner;
    DAComponentInner(Type *_owner)
    {
        ref_count = 1;
        owner = _owner;
    }
    DACOM_DEFMETHOD(QueryInterface)(const C8 *interface_name, void **instance);
    DACOM_DEFMETHOD_(U32, AddRef)(void);
    DACOM_DEFMETHOD_(U32, Release)(void);
};

template <class Type>
GENRESULT DAComponentInner<Type>::QueryInterface(const C8 *interface_name, void **instance)
{
    int i;
    const _DACOM_INTMAP_ENTRY *interfaces = Type::_GetEntriesIn();
    for (i = 0; interfaces[i].interface_name; i++)
    {
        if (strcmp(interfaces[i].interface_name, interface_name) == 0)
        {
            IDAComponent *result;
            if (interfaces[i].offset & 0x80000000)
                result = *((IDAComponent **)(((char *)owner) + (interfaces[i].offset & ~0x80000000)));
            else
                result = (IDAComponent *)(((char *)owner) + interfaces[i].offset);
            result->AddRef();
            *instance = result;
            return GR_OK;
        }
    }
    *instance = 0;
    return GR_INTERFACE_UNSUPPORTED;
}

template <class Type>
U32 DAComponentInner<Type>::AddRef(void)
{
    return owner->getBase()->AddRef();
}

template <class Type>
U32 DAComponentInner<Type>::Release(void)
{
    return owner->getBase()->Release();
}

// ---- COMPTR smart pointer (TSmartPointer.h) ----
template <class Type>
class COMPTR
{
public:
    Type *ptr;
    COMPTR(void) { ptr = 0; }
    ~COMPTR(void) { free(); }
    void free(void)
    {
        if (ptr)
        {
            ptr->Release();
            ptr = 0;
        }
    }
    Type *operator=(Type *new_ptr)
    {
        if (ptr != new_ptr)
        {
            free();
            if ((ptr = new_ptr) != 0)
                ptr->AddRef();
        }
        return (Type *)ptr;
    }
    operator void **(void)
    {
        free();
        return (void **)&ptr;
    }
    operator Type **(void)
    {
        free();
        return (Type **)&ptr;
    }
    operator Type *(void) const { return (Type *)ptr; }
    Type *operator->(void) const { return (Type *)ptr; }
};

//==========================================================================//
// SystemContainer component (SysContainer.cpp)
//==========================================================================//

struct SystemContainer;

HINSTANCE hInstance; // DLL instance handle
ICOManager *DACOM;   // Handle to component manager

struct SysConInner : public DAComponentInner<SystemContainer>
{
    SysConInner(SystemContainer *_owner) : DAComponentInner<SystemContainer>(_owner) {}
    DACOM_DEFMETHOD(QueryInterface)(const C8 *interface_name, void **instance);
};

#define CLSID_SystemContainer "SystemContainer"

struct SystemContainer : public ISystemContainer, IDAConnectionPointContainer
{
    struct ELEMENT
    {
        struct ELEMENT *pNext;
        COMPTR<IDAComponent> pInner;
        IDAComponent *pOuter;
        ISystemComponent *pSysComp;
        IAggregateComponent *pAggComp;
        ELEMENT(void) { pNext = 0; }
    };

    BEGIN_DACOM_MAP_INBOUND(SystemContainer)
    DACOM_INTERFACE_ENTRY2(IID_ISystemContainer, ISystemContainer)
    DACOM_INTERFACE_ENTRY2(IID_IAggregateComponent, IAggregateComponent)
    DACOM_INTERFACE_ENTRY2(IID_ISystemComponent, ISystemComponent)
    DACOM_INTERFACE_ENTRY2(IID_IDAConnectionPointContainer, IDAConnectionPointContainer)
    END_DACOM_MAP()

    ELEMENT *pList, *pLast;
    SysConInner innerComponent;
    IDAComponent *outerComponent;
    BOOL32 bAggMember, bLoaded;

    SystemContainer(void) : innerComponent(this)
    {
        outerComponent = &innerComponent;
    }

    ~SystemContainer(void);

    GENRESULT init(AGGDESC *info);

    DA_HEAP_DEFINE_NEW_OPERATOR(SystemContainer);

    DACOM_DEFMETHOD(QueryInterface)(const C8 *interface_name, void **instance);
    DACOM_DEFMETHOD_(U32, AddRef)(void);
    DACOM_DEFMETHOD_(U32, Release)(void);

    DACOM_DEFMETHOD(LoadSystemComponents)(void);
    DACOM_DEFMETHOD(Shutdown)(void);
    DACOM_DEFMETHOD(AddComponent)(const AGGDESC *descriptor);
    DACOM_DEFMETHOD(Initialize)(void);
    DACOM_DEFMETHOD_(void, Update)(void);
    DACOM_DEFMETHOD(FindConnectionPoint)(const C8 *connectionName, struct IDAConnectionPoint **connPoint);
    DACOM_DEFMETHOD_(BOOL32, EnumerateConnectionPoints)(CONNCONTAINER_ENUM_PROC proc, void *context);

    IDAComponent *getBase(void) { return static_cast<ISystemContainer *>(this); }
};

GENRESULT SystemContainer::QueryInterface(const C8 *interface_name, void **instance)
{
    return QueryInterface(interface_name, instance);
}

U32 SystemContainer::AddRef(void)
{
    return outerComponent->AddRef();
}

U32 SystemContainer::Release(void)
{
    return outerComponent->Release();
}

GENRESULT SysConInner::QueryInterface(const C8 *interface_name, void **instance)
{
    GENRESULT result;

    if ((result = DAComponentInner<SystemContainer>::QueryInterface(interface_name, instance)) == GR_OK)
        return result;

    //
    // Search all available ISystemComponents in order of decreasing
    // priority until one is found which supports the requested application
    // interface
    //

    SystemContainer::ELEMENT *tmp = owner->pList;

    while (tmp)
    {
        if ((result = tmp->pInner->QueryInterface(interface_name, instance)) == GR_OK)
            return result;

        tmp = tmp->pNext;
    }

    return GR_INTERFACE_UNSUPPORTED;
}

GENRESULT SystemContainer::init(AGGDESC *info)
{
    if (info->description != 0 && memcmp(info->description, "SystemContainer", 16) == 0)
        return GR_GENERIC;
    if (info->outer)
    {
        outerComponent = info->outer;
        *(info->inner) = &innerComponent;
        bAggMember = 1;
    }
    return GR_OK;
}

SystemContainer::~SystemContainer(void)
{
    AddRef();
    Shutdown();
}

GENRESULT SystemContainer::Shutdown(void)
{
    ELEMENT *tmp;
    while (pList)
    {
        tmp = pList->pNext;
        delete pList;
        pList = tmp;
    }
    pLast = 0;
    bLoaded = 0;
    return GR_OK;
}

GENRESULT SystemContainer::LoadSystemComponents(void)
{
    if (bLoaded == 0)
    {
        COMPTR<IProfileParser> parser;
        HANDLE hSection;
        char buffer[256];
        int line = 0;

        if (DACOM->QueryInterface(IID_IProfileParser, parser) != GR_OK)
            goto Done;
        if ((hSection = parser->CreateSection("System", 3)) == 0)
            goto Done;

        while (parser->ReadProfileLine(hSection, line++, buffer, sizeof(buffer)) != 0)
        {
            char *ptr, *ptr2;
            AGGDESC info;

            ptr = buffer;
            while (*ptr == ' ' || *ptr == '\t')
                ptr++;
            if (*ptr == ';' || *ptr == 0)
                continue;
            if ((ptr2 = strchr(ptr, '=')) != 0)
            {
                *ptr2++ = 0;
                while (*ptr2 == ' ' || *ptr2 == '\t')
                    ptr2++;
            }

            ELEMENT *element = new ELEMENT;

            info.interface_name = ptr;
            info.outer = getBase();
            info.inner = element->pInner;
            info.description = ptr2;

            if ((ptr = (char *)strchr(info.interface_name, ' ')) != 0)
                *ptr = 0;
            if ((ptr = (char *)strchr(info.interface_name, '\t')) != 0)
                *ptr = 0;

            const char *response = "";

            if (DACOM->CreateInstance(&info, (void **)&element->pOuter) != GR_OK)
            {
                response = "[FAILED]";
                delete element;
            }
            else
            {
                response = "[OK]";
                if (pLast)
                    pLast->pNext = element;
                else
                    pList = element;
                pLast = element;

                if (element->pInner->QueryInterface(IID_ISystemComponent, (void **)&element->pSysComp) == GR_OK)
                    element->pSysComp->Release();
                if (element->pInner->QueryInterface(IID_IAggregateComponent, (void **)&element->pAggComp) == GR_OK)
                    element->pAggComp->Release();
            }

            GENERAL_NOTICE(TEMPSTR("SystemContainer: LoadSystemComponents: Loading '%s' [%s] returned %s\n",
                                   info.interface_name,
                                   info.description ? info.description : "",
                                   response));
        }

        if (bAggMember == 0)
            Initialize();
        bLoaded = 1;
    }
Done:
    return GR_OK;
}

GENRESULT SystemContainer::AddComponent(const AGGDESC *descriptor)
{
    ELEMENT *element = 0;
    AGGDESC *info = (AGGDESC *)descriptor;
    GENRESULT result = GR_OK;
    IDAComponent *outer;
    IDAComponent **inner;

    if (descriptor == 0)
    {
        result = GR_INVALID_PARMS;
        goto Done;
    }

    outer = info->outer; // save these values
    inner = info->inner;

    if ((element = new ELEMENT) == 0)
    {
        result = GR_OUT_OF_MEMORY;
        goto Done;
    }

    info->outer = getBase();
    info->inner = element->pInner;

    if ((result = DACOM->CreateInstance(info, (void **)&element->pOuter)) != GR_OK)
    {
        info->outer = outer; // restore these values
        info->inner = inner;
        goto Done;
    }

    //
    // hook it into the list
    //

    if (pLast)
    {
        pLast->pNext = element;
        pLast = element;
    }
    else
        pLast = pList = element;

    if (element->pInner->QueryInterface("ISystemComponent", (void **)&element->pSysComp) == GR_OK)
    {
        element->pSysComp->Release(); // get rid of extra reference
    }
    if (element->pInner->QueryInterface("IAggregateComponent", (void **)&element->pAggComp) == GR_OK)
    {
        element->pAggComp->Release(); // get rid of extra reference
    }

    info->outer = outer; // restore these values
    info->inner = inner;

Done:
    if (result != GR_OK)
        delete element;
    return result;
}

void SystemContainer::Update(void)
{
    ELEMENT *tmp;

    tmp = pList;
    while (tmp)
    {
        if (tmp->pSysComp)
            tmp->pSysComp->Update();
        tmp = tmp->pNext;
    }
}

GENRESULT SystemContainer::Initialize(void)
{
    ELEMENT *tmp, *back = 0;
    GENRESULT result = GR_OK;

    tmp = pList;
    while (tmp)
    {
        if (tmp->pAggComp && (result = tmp->pAggComp->Initialize()) != GR_OK)
        {
            if (back)
                back->pNext = tmp->pNext;
            else
                pList = tmp->pNext;

            delete tmp;

            if (bAggMember)
                break;

            tmp = pList;
            back = 0;
            continue;
        }

        back = tmp;
        tmp = tmp->pNext;
    }

    return result;
}

GENRESULT SystemContainer::FindConnectionPoint(const C8 *connectionName, struct IDAConnectionPoint **connPoint)
{
    GENRESULT result = GR_GENERIC;
    COMPTR<IDAConnectionPointContainer> container;
    ELEMENT *tmp = pList;

    while (tmp)
    {
        if (tmp->pInner->QueryInterface(IID_IDAConnectionPointContainer, container) == GR_OK)
        {
            if ((result = container->FindConnectionPoint(connectionName, connPoint)) == GR_OK)
                break;
        }
        tmp = tmp->pNext;
    }
    return result;
}

BOOL32 SystemContainer::EnumerateConnectionPoints(CONNCONTAINER_ENUM_PROC proc, void *context)
{
    BOOL32 result = 1;
    COMPTR<IDAConnectionPointContainer> container;
    ELEMENT *tmp = pList;

    while (tmp)
    {
        if (tmp->pInner->QueryInterface(IID_IDAConnectionPointContainer, container) == GR_OK)
        {
            if ((result = container->EnumerateConnectionPoints(proc, context)) == 0)
                break;
        }
        tmp = tmp->pNext;
    }
    return result;
}

//==========================================================================//
// Component factory + registration (SysContainer.cpp / DllMain.cpp)
//==========================================================================//

template <class ClassType, class DescType>
struct DACOM_NO_VTABLE DAComponentFactoryBase : public IComponentFactory
{
    U32 ref_count;
    const char *className;
    DAComponentFactoryBase(const char *_className)
    {
        ref_count = 1;
        if ((className = _className) == 0)
            className = ClassType::_GetEntriesIn()->interface_name;
    }
    DACOM_DEFMETHOD(QueryInterface)(const C8 *interface_name, void **instance);
    DACOM_DEFMETHOD_(U32, AddRef)(void);
    DACOM_DEFMETHOD_(U32, Release)(void);
};

template <class ClassType, class DescType>
GENRESULT DAComponentFactoryBase<ClassType, DescType>::QueryInterface(const C8 *interface_name, void **instance)
{
    *instance = 0;
    if (strcmp(interface_name, IID_IComponentFactory) != 0)
        return GR_INTERFACE_UNSUPPORTED;
    *instance = this;
    AddRef();
    return GR_OK;
}

template <class ClassType, class DescType>
U32 DAComponentFactoryBase<ClassType, DescType>::AddRef(void)
{
    ref_count++;
    return ref_count;
}

template <class ClassType, class DescType>
U32 DAComponentFactoryBase<ClassType, DescType>::Release(void)
{
    if (ref_count > 0)
        ref_count--;
    if (ref_count == 0)
    {
        ref_count++;
        delete this;
        return 0;
    }
    return ref_count;
}

template <class ClassType, class DescType>
struct DAComponentFactory : public DAComponentFactoryBase<ClassType, DescType>
{
    DAComponentFactory(const char *_className) : DAComponentFactoryBase<ClassType, DescType>(_className)
    {
    }

    /* IComponentFactory methods */

    DACOM_DEFMETHOD(CreateInstance)(DACOMDESC *descriptor, void **instance);
};

template <class ClassType, class DescType>
GENRESULT DAComponentFactory<ClassType, DescType>::CreateInstance(DACOMDESC *descriptor, void **instance)
{
    GENRESULT result = GR_OK;
    ClassType *pNewInstance = 0;
    DescType *lpDesc = (DescType *)descriptor;

    //
    // If unsupported interface requested, fail call
    //

    if ((lpDesc->size != sizeof(*lpDesc)) || strcmp(lpDesc->interface_name, className))
    {
        result = GR_INTERFACE_UNSUPPORTED;
        goto Done;
    }

    //
    // Create an instance of ClassType
    //

    if ((pNewInstance = new ClassType) == 0)
    {
        result = GR_OUT_OF_MEMORY;
        goto Done;
    }

    if ((result = pNewInstance->init(lpDesc)) != GR_OK)
    {
        //
        // initialization failed!
        //
        delete pNewInstance;
        pNewInstance = 0;
    }
Done:
    *instance = pNewInstance;
    return result;
}

extern "C" void Register_SystemContainer(ICOManager *pDACOM)
{
    DAComponentFactory<SystemContainer, AGGDESC> *factory = new DAComponentFactory<SystemContainer, AGGDESC>(CLSID_SystemContainer);
    pDACOM->RegisterComponent(factory, CLSID_SystemContainer, DACOM_LOW_PRIORITY);
    factory->Release();
}

BOOL __stdcall DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    //
    // DLL_PROCESS_ATTACH: Create object server component and register it
    // with DACOM manager
    //
    case DLL_PROCESS_ATTACH:
        hInstance = hinstDLL;
        DisableThreadLibraryCalls(hInstance);
        DACOM = DACOM_Acquire();
        Register_SystemContainer(DACOM_Acquire());
        break;

    //
    // DLL_PROCESS_DETACH: Release DACOM manager instance
    //
    case DLL_PROCESS_DETACH:

        break;
    }
    return TRUE;
}
