#include "MMVII_Geom2D.h"
#include "MMVII_Geom3D.h"
#include "MMVII_Sensor.h"
#include "MMVII_PCSens.h"
#include "CodedTarget.h"
#include "CodedTarget_Tpl.h"
#include "MMVII_2Include_Serial_Tpl.h"

/*   Modularistion
 *   Code extern tel que ellipse
 *   Ellipse => avec centre
 *   Pas de continue
 */


namespace MMVII
{
using namespace  cNS_CodedTarget;

/*  *********************************************************** */
/*                                                              */
/*           cAppliCorrecDistCircTarget                         */
/*                                                              */
/*  *********************************************************** */

struct cSimulProjEllispe
{
     public :
        cPt2dr                mProj3DC;
	std::vector<cPt2dr>   mVProjFr;
	cPt2dr                mCorrecC;
};

class cAppliCorrecDistCircTarget : public cMMVII_Appli
{
     public :

        cAppliCorrecDistCircTarget(const std::vector<std::string> & aVArgs,const cSpecMMVII_Appli & aSpec);

     private :
        int Exe() override;

	void EstimateRay();
	tREAL8  EstimateOneRay(const cSaveExtrEllipe &);

	cSimulProjEllispe  EstimateRealCenter(const cMes1GCP &);
	void               EstimateRealCenter();

        cCollecSpecArg2007 & ArgObl(cCollecSpecArg2007 & anArgObl) override ;
        cCollecSpecArg2007 & ArgOpt(cCollecSpecArg2007 & anArgOpt) override ;

	cPhotogrammetricProject     mPhProj;
        cPt3dr                      mNormal;
	tREAL8                      mRayTarget;

        std::string                 mSpecImIn;
        std::string                 mNameIm;
        cSensorImage *              mSensor;
        cSensorCamPC *              mCamPC;
        cSetMesImGCP                mMesImGCP;
};

cAppliCorrecDistCircTarget::cAppliCorrecDistCircTarget
(
    const std::vector<std::string> & aVArgs,
    const cSpecMMVII_Appli & aSpec
) :
   cMMVII_Appli  (aVArgs,aSpec),
   mPhProj       (*this),
   mNormal       (0,0,1)
{
}

        // cExtract_BW_Target * 
cCollecSpecArg2007 & cAppliCorrecDistCircTarget::ArgObl(cCollecSpecArg2007 & anArgObl)
{
   return
            anArgObl
         << Arg2007(mSpecImIn,"Pattern/file for images",{{eTA2007::MPatFile,"0"},{eTA2007::FileDirProj}})
	 << mPhProj.DPOrient().ArgDirInMand()

   ;
}

cCollecSpecArg2007 & cAppliCorrecDistCircTarget::ArgOpt(cCollecSpecArg2007 & anArgOpt)
{
   return 
                  anArgOpt
	     <<   mPhProj.DPPointsMeasures().ArgDirInputOptWithDef("Std")
             << AOpt2007(mRayTarget,"RayTarget","Ray for target (else estimate automatically)")
		/*
             << AOpt2007(mB,"VisuEllipse","Make a visualisation extracted ellispe & target",{eTA2007::HDV})
             << mPhProj.DPMask().ArgDirInOpt("TestMask","Mask for selecting point used in detailed mesg/output")
             << AOpt2007(mPBWT.mMinDiam,"DiamMin","Minimum diameters for ellipse",{eTA2007::HDV})
             << AOpt2007(mPBWT.mMaxDiam,"DiamMax","Maximum diameters for ellipse",{eTA2007::HDV})
             << AOpt2007(mRatioDMML,"RDMML","Ratio Distance minimal bewteen local max /Diam min ",{eTA2007::HDV})
             << AOpt2007(mVisuLabel,"VisuLabel","Make a visualisation of labeled image",{eTA2007::HDV})
             << AOpt2007(mVisuElFinal,"VisuEllipse","Make a visualisation extracted ellispe & target",{eTA2007::HDV})
             << AOpt2007(mPatHihlight,"PatHL","Pattern for highliting targets in visu",{eTA2007::HDV})
	     */
          ;
}


tREAL8  cAppliCorrecDistCircTarget::EstimateOneRay(const cSaveExtrEllipe & aSEE)
{
     const cMes1GCP &  aGCP =   mMesImGCP.MesGCPOfName(aSEE.mNameCode);
     cPlane3D aPlaneT  = cPlane3D::FromPtAndNormal(aGCP.mPt,mNormal);  // plane of the 3D ground target

     cEllipse aEl = mSensor->EllipseIm2Plane(aPlaneT,aSEE.mEllipse,50);
     return std::sqrt(aEl.LSa()*aEl.LGa());
}

/**  Parse all ellispe and etsimate a median ray
 */
void cAppliCorrecDistCircTarget::EstimateRay()
{
   std::string  aNameE = cSaveExtrEllipe::NameFile(mPhProj,mMesImGCP.MesImInitOfName(mNameIm),true);
   std::vector<cSaveExtrEllipe> aVSEE;
   ReadFromFile(aVSEE,aNameE);

   std::vector<tREAL8> aVRay;

   for (const auto & aSEE : aVSEE)
   {
       // if (!starts_with(aSEE.mNameCode,MMVII_NONE))
       if ( mMesImGCP.NameIsGCP(aSEE.mNameCode))
          aVRay.push_back(EstimateOneRay(aSEE));
   }
   mRayTarget = NonConstMediane(aVRay);

}


cSimulProjEllispe cAppliCorrecDistCircTarget::EstimateRealCenter(const cMes1GCP & aGCP)
{
    cSimulProjEllispe aRes;
    const cPt3dr & aCenterTarget =  aGCP.mPt;
    //  The real projection center
    aRes.mProj3DC = mSensor->Ground2Image(aCenterTarget);

    cPlane3D aPlaneTarget = cPlane3D::FromPtAndNormal(aCenterTarget,mNormal);
    int aNbTeta = 200;

    cEllipse_Estimate  aEEs(aRes.mProj3DC);
    for (int aKTeta =0 ; aKTeta < aNbTeta ; aKTeta++)
    {
         cPt2dr aPPl2 = FromPolar(mRayTarget, (2*M_PI*aKTeta)/aNbTeta);
	 cPt3dr aPGr = aPlaneTarget.FromCoordLoc(TP3z0(aPPl2));

	 cPt2dr aPIm =  mSensor->Ground2Image(aPGr);
         aEEs.AddPt(aPIm);
	 aRes.mVProjFr.push_back(aPIm);
    }

    cEllipse anEl = aEEs.Compute() ;
    cPt2dr  aImEstim = anEl.Center();

    // Delta  = Real-Estim  =>>   REAL = ESTIM+DELTA
    aRes.mCorrecC =  aRes.mProj3DC - aImEstim;
    return aRes;
}

void cAppliCorrecDistCircTarget::EstimateRealCenter()
{
   cSetMesPtOf1Im  aSetMesIm = mMesImGCP.MesImInitOfName(mNameIm);

   for (const auto & aMesIm : aSetMesIm.Measures())
   {
        const cMes1GCP &  aGCP = mMesImGCP.MesGCPOfName(aMesIm.mNamePt);

        cSimulProjEllispe aSPE = EstimateRealCenter(aGCP);
	StdOut()  << "DD=" << aGCP.mNamePt << " " << aSPE.mCorrecC << "\n";
   }
}



int  cAppliCorrecDistCircTarget::Exe()
{
   mPhProj.FinishInit();

   if (RunMultiSet(0,0))  // If a pattern was used, run in // by a recall to itself  0->Param 0->Set
   {
      return ResultMultiSet();
   }

   mNameIm = FileOfPath(mSpecImIn);
   mPhProj.LoadSensor(mNameIm,mSensor,mCamPC,true);

   mPhProj.LoadGCP(mMesImGCP);
   mPhProj.LoadIm(mMesImGCP,*mSensor);

   // mCamPC = mPhProj.AllocCamPC(FileOfPath(mSpecImIn),true);

   StdOut()  << mNameIm << " Fff=" << mCamPC->InternalCalib()->F()  << " "<<  mCamPC->NameImage() << "\n";

   if (! IsInit(&mRayTarget))
   {
       EstimateRay();
   }

   EstimateRealCenter();

   StdOut() << "RAY=" <<  mRayTarget << "\n";

   return EXIT_SUCCESS;
}

/* =============================================== */
/*                                                 */
/*                       ::                        */
/*                                                 */
/* =============================================== */

tMMVII_UnikPApli Alloc_DistCorrectCirgTarget(const std::vector<std::string> &  aVArgs,const cSpecMMVII_Appli & aSpec)
{
   return tMMVII_UnikPApli(new cAppliCorrecDistCircTarget(aVArgs,aSpec));
}

cSpecMMVII_Appli  TheSpecDistCorrectCirgTarget
(
     "CodedTargetRefineCirc",
      Alloc_DistCorrectCirgTarget,
      "Refine circ target with shape-distorsion using 3d-predict",
      {eApF::ImProc,eApF::CodedTarget},
      {eApDT::Image,eApDT::Xml},
      {eApDT::Xml},
      __FILE__
);


};

