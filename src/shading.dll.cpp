// shading.dll - matching decompilation (Visual C++ 6.0)
// Single translation unit.
//
// The first functions belong to the AtmosphereMaterial component and its
// DAComponentFactory<DAComponent<AtmosphereMaterial>,DACOMDESC> factory:
//     DECLARE_MATERIAL( AtmosphereMaterial, IS_SIMPLE );

typedef unsigned int U32;
typedef char C8;

void *__cdecl operator new(unsigned int);
extern "C" void __cdecl _free(void *);
extern "C" int __cdecl strcmp(const char *, const char *);
extern "C" int __cdecl memcmp(const void *, const void *, unsigned int);
extern "C" void *__cdecl memset(void *, int, unsigned int);
extern "C" __declspec(dllimport) struct ICOManager *__cdecl DACOM_Acquire(void);

// ---- DACom interface tables (plain structs, explicit vtables) ----

struct IDAComponent;
struct IDAComponent_vtbl
{
    int (__stdcall *QueryInterface)(IDAComponent *, const C8 *, void **);
    U32 (__stdcall *AddRef)(IDAComponent *);
    U32 (__stdcall *Release)(IDAComponent *);
};
struct IDAComponent
{
    IDAComponent_vtbl *vtbl;
};

struct ICOManager_vtbl
{
    int (__stdcall *QueryInterface)(ICOManager *, const C8 *, void **);
    U32 (__stdcall *AddRef)(ICOManager *);
    U32 (__stdcall *Release)(ICOManager *);
    int (__stdcall *CreateInstance)(ICOManager *, void *, void **);
    int (__stdcall *RegisterComponent)(ICOManager *, void *, const C8 *, U32);
};
struct ICOManager
{
    ICOManager_vtbl *vtbl;
};

// descriptor passed to CreateInstance (20 bytes)
struct DACOMDESC
{
    U32 size;                    // 0x00
    const char *interface_name;  // 0x04
    void *outer;                 // 0x08
    void **inner;                // 0x0C
    U32 _unknown10;              // 0x10
};

// one texture option returned by get_texture_options (0x4C bytes)
struct TexOption
{
    U32 type;        // 0x00
    void *name;      // 0x04
    void *desc;      // 0x08
    void *f0C;       // 0x0C
    char Value[0x3C];// 0x10 .. 0x4C
};

// one texture slot allocated by initialize (0x2C bytes)
struct TexSlot
{
    U32 unknown0;   // 0x00
    U32 unknown4;   // 0x04
    U32 unknown8;   // 0x08
    U32 unknownC;   // 0x0C
    U32 unknown10;  // 0x10
    U32 unknown14;  // 0x14
    U32 unknown18;  // 0x18
    U32 unknown1C;  // 0x1C
    U32 unknown20;  // 0x20
    U32 unknown24;  // 0x24
    char unknown28; // 0x28
    char pad[3];    // -> 0x2C
};

// AtmosphereMaterial object (0x88 bytes).  vtable pointer is implicit (+0x00).
struct Material
{
    virtual int __stdcall vf00_QueryInterface(const C8 *, void **) = 0; // 0x00
    virtual U32 __stdcall vf04_AddRef() = 0;                            // 0x04
    virtual U32 __stdcall vf08_Release() = 0;                           // 0x08
    virtual int __stdcall vf0C_initialize(IDAComponent *) = 0;          // 0x0C
    virtual void __stdcall vf10() = 0;                                  // 0x10
    virtual void __stdcall vf14() = 0;                                  // 0x14
    virtual void __stdcall vf18() = 0;                                  // 0x18
    virtual void __stdcall vf1C() = 0;                                  // 0x1C
    virtual void __stdcall vf20() = 0;                                  // 0x20
    virtual void __stdcall vf24() = 0;                                  // 0x24
    virtual void __stdcall vf28() = 0;                                  // 0x28
    virtual void __stdcall vf2C() = 0;                                  // 0x2C
    virtual void __stdcall vf30() = 0;                                  // 0x30
    virtual void __stdcall vf34() = 0;                                  // 0x34
    virtual void __stdcall vf38() = 0;                                  // 0x38
    virtual void __stdcall vf3C() = 0;                                  // 0x3C
    virtual void __stdcall vf40() = 0;                                  // 0x40
    virtual void __stdcall vf44() = 0;                                  // 0x44
    virtual void __stdcall vf48() = 0;                                  // 0x48
    virtual void __stdcall vf4C() = 0;                                  // 0x4C
    virtual void __stdcall vf50() = 0;                                  // 0x50
    virtual void vf54_dtor(int) = 0;                                    // 0x54 (thiscall)
    virtual int vf58_init(DACOMDESC *) = 0;                             // 0x58 (thiscall)
    virtual void __stdcall vf5C() = 0;                                  // 0x5C
    virtual void __stdcall vf60() = 0;                                  // 0x60
    virtual void __stdcall vf64() = 0;                                  // 0x64
    virtual void vf68(U32, void *, void *) = 0;                         // 0x68 (thiscall)
    virtual U32 vf6C_get_texture_options(void **) = 0;                  // 0x6C (thiscall)
    virtual void **vf70(int) = 0;                                       // 0x70 (thiscall)

    void *render_pipeline;      // 0x04
    void *texture_library;      // 0x08
    void *vbuffer_manager;      // 0x0C
    void *render_pipeline_draw; // 0x10
    U32 f14;                    // 0x14
    void *texture_array;        // 0x18
    U32 num_textures;           // 0x1C
    float d3d_material[17];     // 0x20 .. 0x63 (D3DMATERIAL7)
    char f64;                   // 0x64
    char f65;                   // 0x65
    char f66;                   // 0x66
    char f67;                   // 0x67
    void *f68;                  // 0x68
    U32 f6C;                    // 0x6C
    U32 f70;                    // 0x70
    U32 f74;                    // 0x74
    void *outerComponent;       // 0x78
    void *inner_vtbl;           // 0x7C
    U32 inner_ref;              // 0x80
    Material *inner_owner;      // 0x84
};

// AtmosphereMaterial factory object:
//   +0x00 vtbl   +0x04 ref_count   +0x08 className
struct AtmosphereFactory
{
    IDAComponent_vtbl *vtbl;
    U32 ref_count;
    const char *className;
};

// embedded inner component (DAComponentInner) at material+0x7C
struct InnerComponent
{
    void *vtbl;   // 0x00
    U32 ref;      // 0x04
    void *owner;  // 0x08
};

// data symbols (material/inner vtables)
extern "C" void *off_6ED11F0[]; // AtmosphereMaterial vtable
extern "C" void *off_6ED1264[]; // inner (IMaterialProperties) vtable

// forward decls
extern "C" int __stdcall sub_6EB1050(IDAComponent *, const C8 *, void **);
extern "C" U32 __stdcall sub_6EB1090(void *);
extern "C" int __stdcall sub_6EB10C0(AtmosphereFactory *, DACOMDESC *, void **);
static U32 __stdcall AtmosphereFactory_AddRef(IDAComponent *this_);

struct AtmosphereFactory_vtbl_t
{
    int (__stdcall *QueryInterface)(IDAComponent *, const C8 *, void **);
    U32 (__stdcall *AddRef)(IDAComponent *);
    U32 (__stdcall *Release)(void *);
    int (__stdcall *CreateInstance)(AtmosphereFactory *, DACOMDESC *, void **);
};

// ??_7?$DAComponentFactory@U?$DAComponent@UAtmosphereMaterial@@@@UDACOMDESC@@@@6B@
static AtmosphereFactory_vtbl_t AtmosphereFactory_vtbl =
{
    sub_6EB1050,
    AtmosphereFactory_AddRef,
    sub_6EB1090,
    sub_6EB10C0,
};

// DECLARE_MATERIAL( AtmosphereMaterial, IS_SIMPLE )
extern "C" char Register_AtmosphereMaterial(void)
{
    AtmosphereFactory *f = (AtmosphereFactory *)operator new(0xC);
    if (f)
    {
        f->ref_count = 1;
        f->className = "AtmosphereMaterial";
        f->vtbl = (IDAComponent_vtbl *)&AtmosphereFactory_vtbl;
        ICOManager *m = DACOM_Acquire();
        m->vtbl->RegisterComponent(m, f, "AtmosphereMaterial", 0x80000000);
        f->vtbl->Release((IDAComponent *)f);
        return 1;
    }
    return 0;
}

// DAComponentFactoryBase<...>::QueryInterface
extern "C" int __stdcall sub_6EB1050(IDAComponent *this_, const C8 *interface_name, void **instance)
{
    *instance = 0;
    if (memcmp(interface_name, "1.11_IComponentFactory", sizeof("1.11_IComponentFactory")) != 0)
        return -3;
    *instance = this_;
    (*(U32(__stdcall **)(IDAComponent *))(*(int *)this_ + 4))(this_); // AddRef
    return 0;
}

// DAComponentFactoryBase<...>::Release
extern "C" U32 __stdcall sub_6EB1090(void *Block)
{
    U32 *p = (U32 *)Block;
    if (p[1] > 0)
        p[1]--;
    if (p[1] == 0)
    {
        p[1] = 1;
        _free(Block);
        return 0;
    }
    return p[1];
}

// DAComponentFactory<...>::CreateInstance
extern "C" int __stdcall sub_6EB10C0(AtmosphereFactory *this_, DACOMDESC *descriptor, void **instance)
{
    int result = 0;
    Material *pNew = 0;

    if (descriptor->size != 0x14 || strcmp(descriptor->interface_name, this_->className))
    {
        result = -3;
        goto Done;
    }
    pNew = (Material *)operator new(0x88);
    if (pNew != 0)
    {
        pNew->render_pipeline_draw = 0;
        pNew->render_pipeline = 0;
        pNew->texture_library = 0;
        pNew->vbuffer_manager = 0;
        pNew->f14 = 0;
        pNew->texture_array = 0;
        pNew->num_textures = 0;
        pNew->f64 = 0;
        pNew->f65 = 0;
        memset(pNew->d3d_material, 0, 0x44);
        pNew->d3d_material[4] = 1.0f;
        pNew->d3d_material[5] = 1.0f;
        pNew->d3d_material[6] = 1.0f;
        pNew->d3d_material[7] = 1.0f;
        pNew->d3d_material[0] = 1.0f;
        pNew->d3d_material[1] = 1.0f;
        pNew->d3d_material[2] = 1.0f;
        pNew->d3d_material[3] = 1.0f;
        InnerComponent *inner = (InnerComponent *)&pNew->inner_vtbl;
        inner->vtbl = off_6ED1264;
        inner->ref = 1;
        inner->owner = pNew;
        *(void **)pNew = off_6ED11F0;
        if ((pNew->outerComponent = descriptor->outer) == 0)
            pNew->outerComponent = inner;
        else
            *descriptor->inner = inner;
        result = pNew->vf58_init(descriptor);
        if (result != 0)
        {
            if (descriptor->inner)
                *descriptor->inner = 0;
            pNew->vf54_dtor(1);
            pNew = 0;
        }
    }
    else
    {
        result = -4;
    }
Done:
    *instance = pNew;
    return result;
}

// AtmosphereMaterial::initialize  (UnknownMaterial::initialize)
extern "C" int __stdcall UnknownMaterial_initialize(Material *this_, IDAComponent *system_container)
{
    if (system_container == 0)
        return -1;

    if (system_container->vtbl->QueryInterface(system_container, "1.11_ITextureLibraryA", &this_->texture_library) < 0)
        return -1;
    ((IDAComponent *)this_->texture_library)->vtbl->Release((IDAComponent *)this_->texture_library);

    if (system_container->vtbl->QueryInterface(system_container, "1.11_IRenderPipeline8B", &this_->render_pipeline) < 0)
        return -1;
    ((IDAComponent *)this_->render_pipeline)->vtbl->Release((IDAComponent *)this_->render_pipeline);

    if (system_container->vtbl->QueryInterface(system_container, "1.11_IRPDraw", &this_->render_pipeline_draw) < 0)
        return -1;
    ((IDAComponent *)this_->render_pipeline_draw)->vtbl->Release((IDAComponent *)this_->render_pipeline_draw);

    if (system_container->vtbl->QueryInterface(system_container, "1.11_IVertexBufferManager", &this_->vbuffer_manager) < 0)
        return -1;
    ((IDAComponent *)this_->vbuffer_manager)->vtbl->Release((IDAComponent *)this_->vbuffer_manager);

    TexOption *options;
    U32 count = this_->vf6C_get_texture_options((void **)&options);
    this_->num_textures = 0;
    if (count)
    {
        char *q = (char *)&options->Value;
        U32 n = count;
        do
        {
            if (*(U32 *)q == 7)
                ++this_->num_textures;
            q += 0x4C;
            --n;
        } while (n);
    }

    U32 nt = this_->num_textures;
    if (nt)
    {
        char *v9 = (char *)operator new(sizeof(TexSlot) * nt + 4);
        TexSlot *arr;
        if (v9)
        {
            arr = (TexSlot *)(v9 + 4);
            *(U32 *)v9 = nt;
            TexSlot *e = (TexSlot *)(v9 + 4);
            if ((int)(nt - 1) >= 0)
            {
                U32 k = nt;
                do
                {
                    e->unknown0 = 0;
                    e->unknown4 = (U32)-1;
                    e->unknown8 = 0;
                    e->unknown1C = 4;
                    e->unknown20 = 0;
                    e->unknown24 = 0;
                    e->unknown28 = 0;
                    ++e;
                    --k;
                } while (k);
            }
        }
        else
        {
            arr = 0;
        }
        this_->texture_array = arr;
    }

    U32 idx = 0;
    if (count)
    {
        U32 off = 0;
        do
        {
            TexOption *opt = (TexOption *)((char *)options + off);
            this_->vf68(idx, opt, (char *)opt + 0x10);
            ++idx;
            off += 0x4C;
        } while (idx < count);
    }
    return 0;
}

// DAComponentFactoryBase<...>::AddRef
static U32 __stdcall AtmosphereFactory_AddRef(IDAComponent *this_)
{
    return ++((AtmosphereFactory *)this_)->ref_count;
}
