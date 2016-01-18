#ifndef PLOTUTILS_H
#define PLOTUTILS_H

/*
  Author: Marco Valente
  Institute: Ecole Polytechnique Federale de Lausanne
  Email: marco.valente@epfl.ch

  Name: PlotUtils
  Description: this class provides a set of root plot
    tools for RooPlot*, TH1, etc.

  Date: 02/10/2015
 */

//To do:
//    2) Implement color filling of fit curves
//    3) Implement the adjusting of axis labels

#include "TObject.h"

#include <vector>
#include <iostream>
#include <sstream>
//Roofit
#include "RooPlot.h"
#include "TH1.h"
#include "TCanvas.h"

using namespace std;

class ColorPalette {
    public:
        ColorPalette();
        ColorPalette(const char * name);
        ColorPalette(const ColorPalette& c);

        bool                 LoadPalette(const char * name);

        Int_t                GetNConts()    {return _NConts;}
        void                 SetNConts(Int_t nCont)    {_NConts=nCont;}
        Int_t                GetNRGBs()     {return _NRGBs; }
        Double_t *           GetStops()     {return _stops; }
        Double_t *           GetRed()       {return _red;   }
        Double_t *           GetGreen()     {return _green; }
        Double_t *           GetBlue()      {return _blue;  }

    private:
        const char *    _namepalette;
        Int_t           _NConts;
        Int_t           _NRGBs;
        Double_t *      _stops;
        Double_t *      _red;
        Double_t *      _green;
        Double_t *      _blue;

};

class PlotUtils : public TObject {
    public:
        PlotUtils();
        PlotUtils(RooPlot * plot,const char * titleplot = "",
            const char * xlabel=0, const char * ylabel=0);
        PlotUtils(TH1 * hist,const char * titlehist = "",
                const char * xlabel=0, const char * ylabel=0);
        //PlotUtils(RooWorkspace)
        inline virtual ~PlotUtils() {_rooplots.clear(); }

        void              Reset(); // Reset all the arguments

        void              AddRooPlot(RooPlot * plot, const char * titleplot = "",
                                     const char * xlabel=0,const char * ylabel=0);
        void              AddHist(TH1 * hist, const char * titlehist = "",
                                     const char * xlabel=0,const char * ylabel=0);

        // RooPlot utils
        // Add Text box with content of eack variable
        //                  ytop
        // ----------------------------------------
        // -                                      -
        // - xleft                         xright -
        // -                                      -
        // ----------------------------------------
        //                 ydown
        void              AddData(Double_t xleft, Double_t ydown, Double_t xright,
                                  Double_t ytop, const char * var1,
                                  const char * var2=0, const char * var3=0,
                                  const char * var4=0, const char * var5 =0,
                                  const char * var6=0, const char * var7 =0,
                                  const char * var8=0);
        // Add LHCb box data
        void              AddTextLHCb(Int_t year=2012,Double_t xleft = 0.15,
                                      Double_t ydown=0.15);
        void              AddLegend(Double_t xleft, Double_t ydown, Double_t xright, Double_t ytop);

        // Histograms utils
        void              SetStats(bool value=false);
        bool              SetPalette(const char * name = "rainbow", Int_t nContour = -1);
        bool              FillHistograms(const char * name = "rainbow", Float_t alpha = 1.0);
        TCanvas *         PlotAllHist(const char * option = "", const char * canvas_name = "canvas");
        TCanvas *         PlotHistSameCanvas(const char * option = "", const char * canvas_name = "canvas");
        // void AddResiduals();


        void              DrawVerticalLine(Double_t x_pos, Color_t color = 1, Style_t style=9);
        void              DrawHorizontalLine(Double_t y_pos, Color_t color = 1, Style_t style=9);
        //void FillCurves(const char * names,const char * color_palette = "default");

        // Label = numbers for axis
        // Title = title of the axis
        void              SetTitle(const char * title);
        void              SetXTitle(const char * xlabel);
        void              SetXTitleSize(Float_t size = 0.04);
        void              SetXTitleOffset(Float_t offset = 1.);
        void              SetXLabelSize(Float_t size = 0.04);
        void              SetXLabelOffset(Float_t offset = 0.005);
        void              SetYTitle(const char * ylabel);
        void              SetYTitleSize(Float_t size = 0.04);
        void              SetYTitleOffset(Float_t offset = 1.);
        void              SetYLabelSize(Float_t size = 0.04);
        void              SetYLabelOffset(Float_t offset = 0.005);

        void              SetYLabelBinLenght(const char * ylabel = "N_{ev}", const char * bin_units = 0);


        //Operators
        RooPlot& operator[](size_t index) const;

    protected:
        //Arguments
        vector<RooPlot*>    _rooplots;
        vector<TH1*>        _hist;
        ColorPalette        _palette;
        TCanvas *           _canvas;

        //Class definition root
        ClassDef(PlotUtils,1)
};



#endif
