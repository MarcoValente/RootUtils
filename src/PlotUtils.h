/**
* @Author: Marco Valente <marco>
* @Date:   2016-04-07T21:16:35+02:00
* @Email:  marco.valente@cern.ch
* @Last modified by:   marcovalente
* @Last modified time: 2016-05-29T16:49:32+02:00
*/

#ifndef PLOTUTILS_H
#define PLOTUTILS_H

// If AtlasStyle.h/.cpp, AtlasLabels.h/.cpp and AtlasUtils.h/.cpp exist in the
// src/ directory, define USE_ATLAS_STYLE as 1 to use atlas plot style. Otherwise,
// simply switch this value to 0.
#define USE_ATLAS_STYLE 0


#include "TObject.h"

#include <vector>
#include <iostream>
#include <sstream>
//Roofit
#include "RooPlot.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TPad.h"

using namespace std;

class ColorPalette {
    public:
        ColorPalette();
        ColorPalette(const char * name);
        ColorPalette(const ColorPalette& c);
        // ~ColorPalette() { delete _namepalette; delete _stops; delete _red; delete _green; delete _blue;};

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
        PlotUtils(bool verbose);
        PlotUtils(RooPlot * plot,const char * titleplot = "",
            const char * xlabel=0, const char * ylabel=0);
        PlotUtils(TH1 * hist,const char * titlehist = "",
                const char * xlabel=0, const char * ylabel=0);
        virtual ~PlotUtils();

        void              Clear(); // Reset all the arguments


        vector<RooPlot*>      GetRooPlots() { return _rooplots; }
        vector<TH1*>          GetHists()    { return _hist;     }
        vector<TGraphErrors*> GetGraphs()   { return _graph;    }

        void              AddRooPlot(RooPlot * plot, const char * titleplot = "",
                                     const char * xlabel=0,const char * ylabel=0);
        void              AddHist(TH1 * hist, const char * titlehist = "",
                                     const char * xlabel=0,const char * ylabel=0);
        void              AddGraph(TGraphErrors * graph, const char * titlegraph = "",
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

        void              AddLegend(Double_t xleft, Double_t ydown, Double_t xright, Double_t ytop, const char * opt = "l");

        // Histograms utils
        void              Sumw2();
        void              SetStats(bool value=false);
        bool              SetPalette(const char * name = "rainbow", Int_t nContour = -1);
        bool              SetLineColors(const char * name = "rainbow");
        bool              SetLineColors(Color_t color);
        bool              SetMarkerColors(const char * name = "rainbow");
        bool              FillHistograms(const char * name = "rainbow", Float_t alpha = 1.0);
        void              SetMarkerStyles(Int_t marker_style=-1);
        void              SetMarkerSize(Double_t scale=1.);
        void              SetSameBinning(Int_t nbins = -1);
        TPad *            GetRatioPlot(Int_t ref_index = 0, const char * label = 0, Double_t y_pos=0., Double_t height=0.3);
        // void              SetOptimialBinWidth(Double_t value);

        // WARNING: Every time you call function which return TCanvas * you have to clear memory with Clear() method.
        TCanvas *         PlotAllHist(const char * option = "", const char * canvas_name = "canvas");
        TCanvas *         PlotHistSameCanvas(const char * option = "", const char * canvas_name = "canvas");
        TCanvas *         PlotNormalizedHistSameCanvas(const char * option = "", const char * canvas_name = "canvas");
        TCanvas *         PlotGraphSameCanvas(const char * option = "P", const char * canvas_name = "canvas");
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
        void              SetXRange(Double_t xmin,Double_t xmax);
        void              SetXSameRange();

        void              SetYTitle(const char * ylabel);
        void              SetYTitleSize(Float_t size = 0.04);
        void              SetYTitleOffset(Float_t offset = 1.);
        void              SetYLabelSize(Float_t size = 0.04);
        void              SetYLabelOffset(Float_t offset = 0.005);
        void              SetYRange(Double_t ymin,Double_t ymax);

        void              SetYLabelBinLenght(const char * ylabel = "N_{ev}", const char * bin_units = 0);

        void              SetLogy();

        //Operators
        RooPlot& operator[](size_t index) const;

        //AtlasStyle methods
        #if USE_ATLAS_STYLE
        void              SetATLASStyle();
        void              AddTextAtlas(Double_t xleft, Double_t ydown,
                                       const char * text = NULL, Float_t scale_size=1.,
                                       Color_t color=1);
        void              AddTextLine(Double_t xleft, Double_t ydown,
                                      const char * text, Float_t scale_size=1.,
                                      Color_t color=1);
        void              FillGraphs();
        // TCanvas *         PlotNormalizedATLASSameCanvas(const char * canvas_name = "canvas");
        TCanvas *         PlotATLASSameCanvas(bool setlogy = false, Int_t ratio_index = -1, const char * ratio_label = 0, const char * canvas_name = "canvas");
        TCanvas *         PlotNormalizedATLASSameCanvas(bool setlogy = false, Int_t ratio_index = -1, const char * ratio_label = 0,
                                                        bool optimize_y_range = false, const char * canvas_name = "canvas");
        // TCanvas *         PlotTSTSystematicsBand()
        #endif

    protected:
        //Arguments
        bool                   _verbose;
        vector<RooPlot*>       _rooplots;
        vector<TH1*>           _hist;
        vector<TGraphErrors*>  _graph;
        ColorPalette           _palette;
        TCanvas *              _canvas;

        //Class definition root
        ClassDef(PlotUtils,1)
};



#endif
