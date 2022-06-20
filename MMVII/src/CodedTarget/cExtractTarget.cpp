#include "CodedTarget.h"
#include "include/MMVII_2Include_Serial_Tpl.h"
#include "include/MMVII_Tpl_Images.h"


// Test git branch

namespace MMVII
{

namespace  cNS_CodedTarget
{


class cRGBImage
{
     public :
        typedef cIm2D<tU_INT1>   tIm1C;  // Type of image for 1 chanel

        cRGBImage(const cPt2di & aSz);

        /// set values iff param are OK,  RGB image are made for visu, not for intensive computation
        void SetRGBPix(const cPt2di & aPix,int aR,int aG,int aB);
        void SetRGBPix(const cPt2di & aPix,const cPt3di &);
        cPt3di GetRGBPix(const cPt2di & aPix) const;

        ///  Alpha =>  1 force colour  , 0 no effect
        void SetRGBPixWithAlpha(const cPt2di & aPix,const cPt3di &,const cPt3dr & aAlpha);
        ///  
        void SetRGBrectWithAlpha(const cPt2di & aPix,int aSzW,const cPt3di & aCoul,const double & aAlpha);

        void SetGrayPix(const cPt2di & aPix,int aGray);


        void ToFile(const std::string & aName);

        tIm1C  ImR(); ///< Accessor
        tIm1C  ImG(); ///< Accessor
        tIm1C  ImB(); ///< Accessor

        static const  cPt3di  Red;
        static const  cPt3di  Green;
        static const  cPt3di  Blue;
        static const  cPt3di  Yellow;
        static const  cPt3di  Magenta;
        static const  cPt3di  Cyan;

     private :
        tIm1C  mImR;
        tIm1C  mImG;
        tIm1C  mImB;
};

const cPt3di cRGBImage::Red(255,0,0);
const cPt3di cRGBImage::Green(0,255,0);
const cPt3di cRGBImage::Blue(0,0,255);
const cPt3di cRGBImage::Yellow(255,255,0);
const cPt3di cRGBImage::Magenta(255,0,255);
const cPt3di cRGBImage::Cyan(0,255,255);

template <class Type> void SetGrayPix(cRGBImage&,const cPt2di & aPix,const cDataIm2D<Type> & aIm,const double & aMul=1.0);
/// Do it for all pix; 
template <class Type> void SetGrayPix(cRGBImage&,const cDataIm2D<Type> & aIm,const double & aMul=1.0);
template <class Type> cRGBImage  RGBImFromGray(const cDataIm2D<Type> & aIm,const double & aMul=1.0);

typename cRGBImage::tIm1C cRGBImage::ImR() {return mImR;}
typename cRGBImage::tIm1C cRGBImage::ImG() {return mImG;}
typename cRGBImage::tIm1C cRGBImage::ImB() {return mImB;}

cRGBImage::cRGBImage(const cPt2di & aSz) :
    mImR (aSz),
    mImG (aSz),
    mImB (aSz)
{
}
void cRGBImage::SetRGBPix(const cPt2di & aPix,int aR,int aG,int aB)
{
    mImR.DIm().SetVTruncIfInside(aPix,aR);
    mImG.DIm().SetVTruncIfInside(aPix,aG);
    mImB.DIm().SetVTruncIfInside(aPix,aB);
}

void cRGBImage::SetRGBPix(const cPt2di & aPix,const cPt3di & aCoul)
{
     SetRGBPix(aPix,aCoul.x(),aCoul.y(),aCoul.z());
}

cPt3di cRGBImage::GetRGBPix(const cPt2di & aPix) const
{
    return cPt3di(mImR.DIm().GetV(aPix),mImG.DIm().GetV(aPix),mImB.DIm().GetV(aPix));
}

void cRGBImage::SetGrayPix(const cPt2di & aPix,int aGray)
{
     SetRGBPix(aPix,aGray,aGray,aGray);
}

void cRGBImage::ToFile(const std::string & aName)
{
    mImR.DIm().ToFile(aName,mImG.DIm(),mImB.DIm());
}

void cRGBImage::SetRGBPixWithAlpha(const cPt2di & aPix,const cPt3di &aCoul,const cPt3dr & aAlpha)
{
      cPt3di aCurC = GetRGBPix(aPix); 

      cPt3di aMix
             (
                 round_ni(aCurC.x()*aAlpha.x() + aCoul.x()*(1.0-aAlpha.x())),
                 round_ni(aCurC.y()*aAlpha.y() + aCoul.y()*(1.0-aAlpha.y())),
                 round_ni(aCurC.z()*aAlpha.z() + aCoul.z()*(1.0-aAlpha.z()))
             );
      SetRGBPix(aPix,aMix);
}

void cRGBImage::SetRGBrectWithAlpha(const cPt2di & aC,int aSzW,const cPt3di & aCoul,const double & aAlpha)
{
    for (const auto & aPix  :  cRect2::BoxWindow(aC,aSzW))
        SetRGBPixWithAlpha(aPix,aCoul,cPt3dr(aAlpha,aAlpha,aAlpha));
}

    ///  ===========  Manipulation from gray images ========================

template <class Type> void SetGrayPix(cRGBImage& aRGBIm,const cPt2di & aPix,const cDataIm2D<Type> & aGrayIm,const double & aMul)
{
    aRGBIm.SetGrayPix(aPix,round_ni(aMul*aGrayIm.GetV(aPix)));
}

template <class Type> void SetGrayPix(cRGBImage& aRGBIm,const cDataIm2D<Type> & aGrayIm,const double & aMul)
{
    for (const auto & aPix : aRGBIm.ImR().DIm())
        SetGrayPix(aRGBIm,aPix,aGrayIm,aMul);
}


template <class Type> cRGBImage  RGBImFromGray(const cDataIm2D<Type> & aGrayIm,const double & aMul)
{
   cRGBImage aRes(aGrayIm.Sz());

   SetGrayPix(aRes,aGrayIm,aMul);

   return aRes;
}

template  void SetGrayPix(cRGBImage&,const cPt2di & aPix,const cDataIm2D<tREAL4> & aIm,const double &);
template  void SetGrayPix(cRGBImage&,const cDataIm2D<tREAL4> & aIm,const double &);
template  cRGBImage  RGBImFromGray(const cDataIm2D<tREAL4> & aGrayIm,const double & aMul);

/*  *********************************************************** */
/*                                                              */
/*             cAppliExtractCodeTarget                          */
/*                                                              */
/*  *********************************************************** */

enum class eResDCT // Result Detect Code Target
{
     Ok,
     Divg,
     LowSym,
     LowBin,
     LowRad
};

class  cDCT
{
     public  :
         cDCT(const cPt2di aPt,cAffineExtremum<tREAL4> & anAffEx) :
             mPix0  (aPt),
             mPt    (anAffEx.StdIter(ToR(aPt),1e-2,3)),
             mState (eResDCT::Ok),
             mSym   (-1),
             mBin   (-1),
             mRad   (-1)
         {
               if ( (anAffEx.Im().Interiority(Pix())<20) || (Norm2(mPt-ToR(aPt))>2.0)  )  
                    mState = eResDCT::Divg;
         }

         cPt2di  Pix()  const {return ToI(mPt);}
         cPt2di  Pix0() const {return mPix0;}

         cPt2di  mPix0;
         cPt2dr  mPt;
         eResDCT mState;

         double  mSym;
         double  mBin;
         double  mRad;
};


class cAppliExtractCodeTarget : public cMMVII_Appli,
	                        public cAppliParseBoxIm<tREAL4>
{
     public :
        cAppliExtractCodeTarget(const std::vector<std::string> & aVArgs,const cSpecMMVII_Appli & aSpec);

     private :
        int Exe() override;
        cCollecSpecArg2007 & ArgObl(cCollecSpecArg2007 & anArgObl) override ;
        cCollecSpecArg2007 & ArgOpt(cCollecSpecArg2007 & anArgOpt) override ;

	int ExeOnParsedBox() override;

	void TestFilters();
	void DoExtract();
        void ShowStats(const std::string & aMes) const;
        void MarkDCT() ;

	std::string mNameTarget;

	cParamCodedTarget        mPCT;
	cPt2dr                   mRaysTF;
        std::vector<eDCTFilters> mTestedFilters;    

        double   mR0Sym;     ///< R min for first very quick selection on symetry
        double   mR1Sym;     ///< R max for first very quick selection on symetry
        double   mRExtreSym; ///< R to compute indice of local maximal of symetry
        double   mTHRS_Sym; ///< Threshold for symetricity


        double mTHRS_Bin;

        std::vector<cDCT>   mVDCT; ///< vector of detected target


        cRGBImage  mImVisu;
        std::string mPatF;
         
};


/* *************************************************** */
/*                                                     */
/*              cAppliExtractCodeTarget                   */
/*                                                     */
/* *************************************************** */

cAppliExtractCodeTarget::cAppliExtractCodeTarget(const std::vector<std::string> & aVArgs,const cSpecMMVII_Appli & aSpec) :
   cMMVII_Appli  (aVArgs,aSpec),
   cAppliParseBoxIm<tREAL4>(*this,true,cPt2di(5000,5000),cPt2di(300,300),false), // static_cast<cMMVII_Appli & >(*this))
   mRaysTF        ({4,8}),
   mR0Sym         (3.0),
   mR1Sym         (8.0),
   mRExtreSym     (7.0),
   mTHRS_Sym      (0.8),
   mTHRS_Bin      (0.5),
   mImVisu        (cPt2di(1,1)),
   mPatF          ("XXX")
{
}

cCollecSpecArg2007 & cAppliExtractCodeTarget::ArgObl(cCollecSpecArg2007 & anArgObl) 
{
   // Standard use, we put args of  cAppliParseBoxIm first
   return
         APBI_ArgObl(anArgObl)
             <<   Arg2007(mNameTarget,"Name of target file")
   ;
}
/* But we could also put them at the end
   return
         APBI_ArgObl(anArgObl <<   Arg2007(mNameTarget,"Name of target file"))
   ;
*/

cCollecSpecArg2007 & cAppliExtractCodeTarget::ArgOpt(cCollecSpecArg2007 & anArgOpt)
{
   return APBI_ArgOpt
	  (
	        anArgOpt
                    << AOpt2007(mRaysTF, "RayTF","Rays Min/Max for testing filter",{eTA2007::HDV,eTA2007::Tuning})
                    << AOpt2007(mPatF, "PatF","Pattern filters" ,{AC_ListVal<eDCTFilters>()})
	  );
   ;
}

void cAppliExtractCodeTarget::ShowStats(const std::string & aMes) const
{
   int aNbOk=0;
   for (const auto & aR : mVDCT)
   {
      if (aR.mState == eResDCT::Ok)
         aNbOk++;
   }
  std::cout <<  aMes << " NB DCT = " << aNbOk << " Prop " << (double) aNbOk / (double) APBI_DIm().NbElem() << "\n";
}

void cAppliExtractCodeTarget::MarkDCT() 
{
     for (auto & aDCT : mVDCT)
     {
          cPt3di aCoul (-1,-1,-1);

          if (aDCT.mState == eResDCT::Ok)      aCoul =  cRGBImage::Green;
          if (aDCT.mState == eResDCT::Divg)    aCoul =  cRGBImage::Red;
          if (aDCT.mState == eResDCT::LowSym)  aCoul =  cRGBImage::Yellow;
          if (aDCT.mState == eResDCT::LowBin)  aCoul =  cRGBImage::Blue;
          if (aDCT.mState == eResDCT::LowRad)  aCoul =  cRGBImage::Cyan;


          if (aCoul.x() >=0)
             mImVisu.SetRGBrectWithAlpha(aDCT.Pix0(),2,aCoul,0.5);
     }
}

void  cAppliExtractCodeTarget::DoExtract()
{
     tDataIm &  aDIm = APBI_DIm();
     tIm        aIm = APBI_Im();
     mImVisu =   RGBImFromGray(aDIm);
     // mNbPtsIm = aDIm.Sz().x() * aDIm.Sz().y();

     // Extract point that are extremum of symetricity
     cIm2D<tREAL4>  aImSym = ImSymetricity(aIm,mR0Sym,mR1Sym,0);
     cResultExtremum aRExtre(true,false);
     ExtractExtremum1(aImSym.DIm(),aRExtre,mRExtreSym);

     cAffineExtremum<tREAL4> anAffEx(aImSym.DIm(),2.0);


     for (const auto & aPix : aRExtre.mPtsMin)
     {
          mVDCT.push_back(cDCT(aPix,anAffEx));
     }
     ShowStats("Init ");

     //   ====   Symetry filters ====
     for (auto & aDCT : mVDCT)
     {
        if (aDCT.mState == eResDCT::Ok)
        {
           aDCT.mSym = aImSym.DIm().GetV(aDCT.Pix());
           if (aDCT.mSym>mTHRS_Sym)
              aDCT.mState = eResDCT::LowSym;  
        }
     }
     ShowStats("LowSym ");

     //   ====   Symetry filters ====
     {
         std::vector<cPt2di>  aVectVois =  VectOfRadius(6,8,false);

         for (auto & aDCT : mVDCT)
         {
             if (aDCT.mState == eResDCT::Ok)
             {
                 aDCT.mBin = IndBinarity(aDIm,aDCT.Pix(),aVectVois);
                 if (aDCT.mBin>mTHRS_Bin)
                    aDCT.mState = eResDCT::LowBin;  
             }
        }
     }
     ShowStats("Binary ");

     //   ====   Radian filters ====
     {
         cImGrad<tREAL4>  aImG = Deriche(aDIm,1.0);
         std::vector<cPt2di>  aVectVois =  VectOfRadius(3.5,5.5,false);
         std::vector<cPt2dr>  aVDir = VecDir(aVectVois);


         for (auto & aDCT : mVDCT)
         {
             if (aDCT.mState == eResDCT::Ok)
             {
                 aDCT.mRad =  Starity (aImG,aDCT.mPt,aVectVois,aVDir,1.0);

                 if (aDCT.mRad>0.5)
                    aDCT.mState = eResDCT::LowRad;  
             }
        }
     }
     ShowStats("Starity ");


     MarkDCT() ;
     mImVisu.ToFile("VisuCodeTarget.tif");
}


void  cAppliExtractCodeTarget::TestFilters()
{
     tDataIm &  aDIm = APBI_DIm();
     tIm        aIm = APBI_Im();

     StdOut() << "SZ "  <<  aDIm.Sz() << " Im=" << APBI_NameIm() << "\n";


     for (const auto & anEF :  mTestedFilters)
     {
            StdOut()  << " F=" << E2Str(anEF) << "\n";

            cIm2D<tREAL4> aImF(cPt2di(1,1));

            if (anEF==eDCTFilters::eBin)
                aImF = ImBinarity(aDIm,mRaysTF.x(),mRaysTF.y(),1.0);

            if (anEF==eDCTFilters::eSym)
                aImF = ImSymetricity(aIm,mRaysTF.x(),mRaysTF.y(),1.0);

            if (anEF==eDCTFilters::eRad)
            {
                cImGrad<tREAL4>  aImG = Deriche(aDIm,1.0);
                aImF = ImStarity(aImG,mRaysTF.x(),mRaysTF.y(),1.0);
            }


            if (aImF.DIm().Sz().x() > 1)
            {
	       std::string aName = "TestDCT_" +  E2Str(anEF)  + "_" + Prefix(mNameIm) + ".tif";
	       aImF.DIm().ToFile(aName);
            }

/*
  
          cIm2D<tREAL4>  aImBin = ImBinarity(aDIm,aDist/1.5,aDist,1.0);
	  std::string aName = "TestBin_" + ToStr(aDist) + "_" + Prefix(APBI_NameIm()) + ".tif";
	  aImBin.DIm().ToFile(aName);
	  StdOut() << "Done Bin\n";
          cIm2D<tREAL4>  aImSym = ImSymetricity(aDIm,aDist/1.5,aDist,1.0);
	  std::string aName = "TestSym_" + ToStr(aDist) + "_" + Prefix(APBI_NameIm()) + ".tif";
	  aImSym.DIm().ToFile(aName);
	  StdOut() << "Done Sym\n";
*/

	  /*
          cIm2D<tREAL4>  aImStar = ImStarity(aImG,aDist/1.5,aDist,1.0);
	  aName = "TestStar_" + ToStr(aDist) + "_" + Prefix(mNameIm) + ".tif";
	  aImStar.DIm().ToFile(aName);
	  StdOut() << "Done Star\n";

          cIm2D<tREAL4>  aImMixte =   aImSym + aImStar * 2.0;
	  aName = "TestMixte_" + ToStr(aDist) + "_" + Prefix(mNameIm) + ".tif";
	  aImMixte.DIm().ToFile(aName);
	  */
     }

}

int cAppliExtractCodeTarget::ExeOnParsedBox()
{
/*
   if (APBI_TestMode())
   {
   }
   else
   {
   }
*/
   TestFilters();
   DoExtract();

   return EXIT_SUCCESS;
}

int  cAppliExtractCodeTarget::Exe()
{
   mTestedFilters = SubOfPat<eDCTFilters>(mPatF,true);
   StdOut()  << " IIIIm=" << APBI_NameIm()   << "\n";

   if (RunMultiSet(0,0))  // If a pattern was used, run in // by a recall to itself  0->Param 0->Set
      return ResultMultiSet();

   mPCT.InitFromFile(mNameTarget);
   APBI_ExecAll();  // run the parse file


   return EXIT_SUCCESS;
}
};


/* =============================================== */
/*                                                 */
/*                       ::                        */
/*                                                 */
/* =============================================== */
using namespace  cNS_CodedTarget;

tMMVII_UnikPApli Alloc_ExtractCodedTarget(const std::vector<std::string> &  aVArgs,const cSpecMMVII_Appli & aSpec)
{
   return tMMVII_UnikPApli(new cAppliExtractCodeTarget(aVArgs,aSpec));
}

cSpecMMVII_Appli  TheSpecExtractCodedTarget
(
     "CodedTargetExtract",
      Alloc_ExtractCodedTarget,
      "Extract coded target from images",
      {eApF::CodedTarget,eApF::ImProc},
      {eApDT::Image,eApDT::Xml},
      {eApDT::Xml},
      __FILE__
);


};
