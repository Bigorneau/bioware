#include "pack.h"

char * resouce_to_extension(enum e_res_type res)
{
	switch (res) {
	case RES_TYPE_RES:
		return "ext";
	case RES_TYPE_BMP:
		return "bmp";
	case RES_TYPE_MVE:
		return "mve";
	case RES_TYPE_TGA:
		return "tga";
	case RES_TYPE_WAV:
		return "wav";
	case RES_TYPE_PLT:
		return "plt";
	case RES_TYPE_INI:
		return "ini";
	case RES_TYPE_MP3:
		return "mp3";
	case RES_TYPE_MPG:
		return "mpg";
	case RES_TYPE_TXT:
		return "txt";
	case RES_TYPE_XML:
		return "xml";
	case RES_TYPE_PLH:
		return "plh";
	case RES_TYPE_TEX:
		return "tex";
	case RES_TYPE_MDL:
		return "mdl";
	case RES_TYPE_THG:
		return "thg";
	case RES_TYPE_FNT:
		return "fnt";
	case RES_TYPE_LUA:
		return "lua";
	case RES_TYPE_SLT:
		return "slt";
	case RES_TYPE_NSS:
		return "nss";
	case RES_TYPE_NCS:
		return "ncs";
	case RES_TYPE_MOD:
		return "mod";
	case RES_TYPE_ARE:
		return "are";
	case RES_TYPE_SET:
		return "set";
	case RES_TYPE_IFO:
		return "ifo";
	case RES_TYPE_BIC:
		return "bic";
	case RES_TYPE_WOK:
		return "wok";
	case RES_TYPE_2DA:
		return "2da";
	case RES_TYPE_TLK:
		return "tlk";
	case RES_TYPE_TXI:
		return "txi";
	case RES_TYPE_GIT:
		return "git";
	case RES_TYPE_BTI:
		return "gti";
	case RES_TYPE_UTI:
		return "uti";
	case RES_TYPE_BTC:
		return "btc";
	case RES_TYPE_UTC:
		return "utc";
	case RES_TYPE_DLG:
		return "dlg";
	case RES_TYPE_ITP:
		return "itp";
	case RES_TYPE_BTT:
		return "btt";
	case RES_TYPE_UTT:
		return "utt";
	case RES_TYPE_DDS:
		return "dds";
	case RES_TYPE_BTS:
		return "bts";
	case RES_TYPE_UTS:
		return "uts";
	case RES_TYPE_LTR:
		return "ltr";
	case RES_TYPE_GFF:
		return "gff";
	case RES_TYPE_FAC:
		return "fac";
	case RES_TYPE_BTE:
		return "bte";
	case RES_TYPE_UTE:
		return "ute";
	case RES_TYPE_BTD:
		return "btd";
	case RES_TYPE_UTD:
		return "utd";
	case RES_TYPE_BTP:
		return "btp";
	case RES_TYPE_UTP:
		return "utp";
	case RES_TYPE_DFT:
		return "dft";
	case RES_TYPE_GIC:
		return "gic";
	case RES_TYPE_GUI:
		return "gui";
	case RES_TYPE_CSS:
		return "css";
	case RES_TYPE_CCS:
		return "ccs";
	case RES_TYPE_BTM:
		return "btm";
	case RES_TYPE_UTM:
		return "utm";
	case RES_TYPE_DWK:
		return "dwk";
	case RES_TYPE_PWK:
		return "fwk";
	case RES_TYPE_BTG:
		return "btg";
	case RES_TYPE_JRL:
		return "jrl";
	case RES_TYPE_SAV:
		return "sav";
	case RES_TYPE_UTW:
		return "utw";
	case RES_TYPE_4PC:
		return "4pc";
	case RES_TYPE_SSF:
		return "ssf";
	case RES_TYPE_BIK:
		return "bik";
	case RES_TYPE_NDB:
		return "ndb";
	case RES_TYPE_PTM:
		return "ptm";
	case RES_TYPE_PTT:
		return "ptt";
	case RES_TYPE_NCM:
		return "ncm";
	case RES_TYPE_MFX:
		return "mfx";
	case RES_TYPE_MAT:
		return "mat";
	case RES_TYPE_MDB:
		return "mdb";
	case RES_TYPE_SAY:
		return "say";
	case RES_TYPE_TTF:
		return "ttf";
	case RES_TYPE_TTC:
		return "ttc";
	case RES_TYPE_CUT:
		return "cut";
	case RES_TYPE_KA:
		return "ka";
	case RES_TYPE_JPG:
		return "jpg";
	case RES_TYPE_ICO:
		return "igo";
	case RES_TYPE_OGG:
		return "ogg";
	case RES_TYPE_SPT:
		return "spt";
	case RES_TYPE_SPW:
		return "sfw";
	case RES_TYPE_WFX:
		return "wfx";
	case RES_TYPE_UGM:
		return "ugm";
	case RES_TYPE_QDB:
		return "qdb";
	case RES_TYPE_QST:
		return "qst";
	case RES_TYPE_NPC:
		return "npc";
	case RES_TYPE_SPN:
		return "spn";
	case RES_TYPE_UTX:
		return "utx";
	case RES_TYPE_MMD:
		return "mmd";
	case RES_TYPE_SMM:
		return "smm";
	case RES_TYPE_UTA:
		return "uta";
	case RES_TYPE_MDE:
		return "mde";
	case RES_TYPE_MDV:
		return "mdv";
	case RES_TYPE_MDA:
		return "mda";
	case RES_TYPE_MBA:
		return "mba";
	case RES_TYPE_OCT:
		return "oct";
	case RES_TYPE_BFX:
		return "bfx";
	case RES_TYPE_PDB:
		return "pdb";
	case RES_TYPE_TWS:
		return "TheWitcherSave";
	case RES_TYPE_PVS:
		return "pvs";
	case RES_TYPE_CFX:
		return "cfx";
	case RES_TYPE_LUC:
		return "luc";
	case RES_TYPE_PRB:
		return "prb";
	case RES_TYPE_CAM:
		return "cam";
	case RES_TYPE_VDS:
		return "vds";
	case RES_TYPE_BIN:
		return "bin";
	case RES_TYPE_WOB:
		return "wob";
	case RES_TYPE_API:
		return "api";
	case RES_TYPE_PROP:
		return "properties";
	case RES_TYPE_PNG:
		return "png";
	case RES_TYPE_BIG:
		return "big";
	case RES_TYPE_ERF:
		return "erf";
	case RES_TYPE_BIF:
		return "bif";
	case RES_TYPE_KEY:
		return "key";
	};
	return "oops";
}
