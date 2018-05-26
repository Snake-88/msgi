#pragma once

#include "MgsFunction.hpp"
#include "Actor.hpp"
#include "Psx.hpp"

void ForceLinkKmdCpp();
void DoKmdTests();

struct KmdHeader;

struct kmdObject
{
    DWORD field_0_flags;
    DWORD field_4_numFaces;
    VECTOR boundingBox[2];
    VECTOR translation;
    int mRef_2C_parentObjIndex;
    int mObjPosNum_30_translationUnk;

    DWORD numVerts_34;
    SVECTOR* vertOfs_38;
    BYTE *indexOfs_3C;

    DWORD numNorms_40;
    SVECTOR* normOfs_44;
    BYTE* normIndex_48;

    BYTE* ofsUV_4C;
    WORD* ofsTextureNameHashes_50;
    DWORD nullpad_54;
};
MGS_ASSERT_SIZEOF(kmdObject, 0x58);

struct Prim_Mesh_0x5C
{
    PSX_MATRIX field_0_mtx;
    PSX_MATRIX field_20_mtx;
    kmdObject* field_40_pKmdObj;
    CVECTOR* field_44_light_colour_buffer; // 16 bytes with rgbp for each POLY_GT4
    Prim_Mesh_0x5C* field_48_pLinked;
    WORD field_4C_bounding_ret;
    WORD field_4E_counter;
    WORD field_50_numObjTranslated;
    WORD field_52_num_faces;
    POLY_GT4* field_54_prim_buffers[2];
};
MGS_ASSERT_SIZEOF(Prim_Mesh_0x5C, 0x5C);

struct kmd_ex_data;

struct Prim_unknown_0x48
{
    PSX_MATRIX field_0_matrix;
    Prim_Mesh_0x5C* field_20_pMesh; // TODO: May actually still be a matrix 
    KmdHeader* field_24_pKmdFileData;
    DWORD field_28_flags_or_type;
    WORD field_2C_index;
    WORD field_2E_UnknownOrNumFaces;
    WORD field_30_size;
    WORD field_32;
    PSX_MATRIX* field_34_light_mtx_array;
    kmd_ex_data* field_38_size24b;
    SVECTOR* field_3C;
    SVECTOR* field_40;
    SVECTOR* field_44;
};
MGS_ASSERT_SIZEOF(Prim_unknown_0x48, 0x48);

struct Prim_24b // TODO: Probably kmd_ex_data ?
{
    SVECTOR field_0_v1;
    SVECTOR field_8_v2;
    SVECTOR field_10_v3;
};
MGS_ASSERT_SIZEOF(Prim_24b, 0x18);

struct Prim_unknown_0x54
{
    PSX_MATRIX field_0_matrix;
    PSX_MATRIX* field_20;
    int field_24_flags2;
    WORD field_28_flags_or_type;
    WORD field_2A_num_prims;
    WORD field_2C_gv_index;
    WORD field_2E_UnknownOrNumFaces;
    WORD field_30_prim_size;
    WORD field_32_primF2_vert_count;
    WORD field_34_primF3;
    WORD field_36_primF4;
    Prim_24b* field_38_size24b;
    void* field_3C;
    BYTE* field_40_pDataStart[2];
    // ^^^ BASE

    WORD field_48_count;
    WORD field_4A;
    DWORD field_4C;
    void(__cdecl* field_50_pFn)(struct Prim_unknown_0x54 *, BYTE*, int);
};
MGS_ASSERT_SIZEOF(Prim_unknown_0x54, 0x54);

union Prim_Union
{
    Prim_unknown_0x54 prim_54;
    Prim_unknown_0x48 prim_48;
};
MGS_ASSERT_SIZEOF(Prim_Union, 0x54);

struct struc_kmd_14
{
    VECTOR v;
};

struct struc_kmd
{
    Prim_Union* field_0_pObj;
    int field_4_size;
    PSX_MATRIX* field_8_light_mtx_array;
    WORD field_C_mapflags_or_script_binds;
    short field_E_anim_id;
    int field_10;
    struc_kmd_14 field_14_struc;
    int field_20;
};
MGS_ASSERT_SIZEOF(struc_kmd, 0x24);

struct kmd_ex_data
{
    SVECTOR field_0[24];
};
MGS_ASSERT_SIZEOF(kmd_ex_data, 0xC0);

struct struct_kmd_ex
{
    struc_kmd field_0_mBase;
    kmd_ex_data field_24_mEx;
};
MGS_ASSERT_SIZEOF(struct_kmd_ex, 0xE4);

struct KmdHeader
{
    DWORD mNumberOfMeshes;
    DWORD mNumberOfObjects;
    VECTOR mBoundingBox[2];
};
MGS_ASSERT_SIZEOF(KmdHeader, 0x20);


void CC Kmd_Load_44FF7C(struc_kmd* pObj, int resHash, int size);
void CC Kmd_free_4500DD(struc_kmd* pKmd);
void CC Object_Remove_4017C3(Prim_unknown_0x48* pPrim);
void CC Prim_free_colour_buffer_443FCB(Prim_unknown_0x48* pPrim);
void CC Prim_void_and_free_4440BE(Prim_unknown_0x48* pPrim);
Prim_Union* CC Obj_Alloc_443FEC(KmdHeader* pFileData, int countOrType_0x40Flag, __int16 usuallyZero);
int CC Object_Add_40178F(Prim_Union* pPrim);
void CC Prim_Remove_401839(Prim_unknown_0x54* pPrimUnknown);
void CC Kmd_Set_Light_matrices_450109(struc_kmd* pKmd, PSX_MATRIX* pLightMtxAry);
void CC Vector_subtract_40B4ED(const SVECTOR* aLhs, const SVECTOR* aRhs, SVECTOR* out);
int CC FixedSubtract_40B6BD(__int16 value1, __int16 value2);
void CC Res_base_unknown_407B3D(const SVECTOR* pVec);
void CC Res_base_unknown_407B79(const SVECTOR* pRotVec);
int CC Res_base_unknown_40241F(const SVECTOR* pVec, PSX_MATRIX* pMtxAry);

struct Light
{
    short int field_0_x;
    short int field_2_y;
    short int field_4_z;
    short int field_6_w; // padding?
    short int field_8_brightness;
    short int field_A_radius;
    CVECTOR field_C_colour;
};
MGS_ASSERT_SIZEOF(Light, 16);

signed int CC PrimObjRelated_443A4E(Prim_unknown_0x48* pObj, const Light* pLights, int lightCount);
int CC Kmd_TotalLightSizeInBytes_443FAF(const KmdHeader* pKmdHeader);
void CC Kmd_Link_To_Parent_Mesh_45011B(struc_kmd* pKmd, struc_kmd* pParentKmd, int idx);

void CC RotMatrixZYX_gte_44C880(const SVECTOR* pVec, MATRIX3x3* pMtx);
void CC RotMatrixY_44C270(__int16 value, PSX_MATRIX* pMtx);
void CC RotMatrixX_44C1C0(__int16 value, PSX_MATRIX* pMatrix);
void CC RotMatrixZ_44C320(__int16 value, PSX_MATRIX* pMatrix);
void CC Vector_unknown_407549(PSX_MATRIX* pMtx, SVECTOR* pVec);
