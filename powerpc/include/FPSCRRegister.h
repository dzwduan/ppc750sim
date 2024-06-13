/***************************************************************************
                          FPSCRRegister.h  -  description
                             -------------------
    begin                : Wed Aug 8 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#ifndef __FPSCRREGISTER_H__
#define __FPSCRREGISTER_H__

#include <systemc.h>

SC_MODULE(FPSCRRegister)
{
	sc_in<bool> inClock;

	sc_in<bool> inWriteFX;
	sc_in<bool> inFX;
	sc_in<bool> inWriteFEX;
	sc_in<bool> inFEX;
	sc_in<bool> inWriteFX;
	sc_in<bool> inFX;
	sc_in<bool> inWriteFX;
	sc_in<bool> inFX;
	sc_in<bool> inWriteFX;
	sc_in<bool> inFX;
	sc_in<bool> inWriteFX;
	sc_in<bool> inFX;
	sc_in<bool> inWriteFX;
	sc_in<bool> inFX;
	sc_in<bool> inWriteFX;
	sc_in<bool> inFX;
	sc_in<bool> inWriteFX;
	sc_in<bool> inFX;
	sc_in<bool> inWriteFX;
	sc_in<bool> inFX;
	sc_in<bool> inWriteFX;
	sc_in<bool> inFX;
	sc_in<bool> inWriteFX;
	sc_in<bool> inFX;
	sc_in<bool> inWriteFX;
	sc_in<bool> inFX;
	sc_in<bool> inWriteFX;
	sc_in<bool> inFX;
	sc_in<bool> inWriteFX;
	sc_in<bool> inFX;
	sc_in<bool> inWriteFX;
	sc_in<bool> inFX;
	sc_in<bool> inWriteFX;
	sc_in<bool> inFX;
	sc_in<bool> inWriteFX;
	sc_in<bool> inFX;
	sc_in<bool> inWriteFX;
	sc_in<bool> inFX;
	sc_in<bool> inWriteFX;
	sc_in<bool> inFX;
	sc_in<bool> inWriteFX;
	sc_in<bool> inFX;
	sc_in<bool> inWriteFX;
	sc_in<bool> inFX;
	sc_in<bool> inWriteFX;
	sc_in<bool> inFX;
	sc_in<bool> inWriteFX;
	sc_in<bool> inFX;
	
	sc_out<bool> outFX;
	sc_out<bool> outFEX;

	bool fx;		/* FPSCR[FX] */
	bool fex;		/* FPSCR[FEX] */
	bool vx;		/* FPSCR[VX] */
	bool ox;		/* FPSCR[OX] */
	bool ux;		/* FPSCR[UX] */
	bool zx;		/* FPSCR[ZX] */
	bool xx;		/* FPSCR[XX] */
	bool vsxnan;	/* FPSCR[VSXNAN] */
	bool vxisi;	/* FPSCR[VXISI] */
	bool vxdi;	/* FPSCR[VXDI] */
	bool vxzdz;	/* FPSCR[VXZDZ] */
	bool vximz;	/* FPSCR[VXIMZ] */
	bool vxvc;	/* FPSCR[VXVC] */
	bool fr;		/* FPSCR[FR] */
	bool fi;		/* FPSCR[FI] */
	bool fprf;		/* FPSCR[FPRF] */
	bool vxsoft;	/* FPSCR[VXSOFT] */
	bool vxsqrt;	/* FPSCR[VXSQRT] */
	bool vxcvi;	/* FPSCR[VXCVI] */
	bool ve;		/* FPSCR[VE] */
	bool oe;		/* FPSCR[OE] */
	bool ue;		/* FPSCR[UE] */
	bool ze;		/* FPSCR[ZE] */
	bool xe;		/* FPSCR[XE] */
	bool ni;		/* FPSCR[NI] */
	bool rn;		/* FPSCR[RN] */
	
	void Write();

	SC_CTOR(XERRegister)
	{
		SC_METHOD(Write);
		sensitive_neg << inClock;
	
		fx = fex = vx = ox = ux = zx = xx = vsxnan =
		vxisi = vxdi = vxzdz = vximz = vxvc = fr = fi =
		fprf = vxsoft = vxsqrt = vxcvi = ve = oe = ue =
		 ze = xe = ni = rn = false;
	}	
};

#endif
