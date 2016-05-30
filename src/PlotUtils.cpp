/**
* @Author: marcovalente
* @Date:   2016-04-24T14:29:16+02:00
* @Last modified by:   marcovalente
* @Last modified time: 2016-05-29T16:50:51+02:00
*/



#include <map>
#include <vector>
#include "PlotUtils.h"

#include "TPaveText.h"
#include "TText.h"
#include "TColor.h"
#include "ConfigParser.h"
#include "TAttFill.h"
#include "TLine.h"
#include "TLegend.h"

#include <boost/math/common_factor_rt.hpp>

#if USE_ATLAS_STYLE
#include "AtlasStyle.h"
#include "AtlasLabels.h"
#include "AtlasUtils.h"
#endif

using namespace RooFit;

std::map<std::string,std::string> ColorList = {
    {"rainbow","/afs/cern.ch/user/v/valentem/script/config/plotconfig/rainbow.conf"},
    {"kbird","/afs/cern.ch/user/v/valentem/script/config/plotconfig/kbird.conf"}
};

std::vector<int> marker_styles = {20,21,22,23,33,29};

std::string tostr(Double_t num) {
    std::ostringstream strs;
    strs << num;
    std::string str = strs.str();
    return str;
}

ClassImp(PlotUtils);

/**
 * Class ColorPalette implementation
 */

ColorPalette::ColorPalette():_namepalette(), _NConts(255), _NRGBs(5) {
    Double_t    stops[_NRGBs];
    Double_t    red[_NRGBs];
    Double_t    green[_NRGBs];
    Double_t    blue[_NRGBs];

    _stops  =    stops;
    _red    =      red;
    _green  =    green;
    _blue   =     blue;
}

ColorPalette::ColorPalette(const ColorPalette& c):_namepalette(c._namepalette),
        _NConts(c._NConts),
        _NRGBs(c._NRGBs),
        _stops(c._stops),
        _red(c._red),
        _green(c._green),
        _blue(c._blue) {}

ColorPalette::ColorPalette(const char * name): _namepalette(name){
    LoadPalette(name);
}

bool ColorPalette::LoadPalette(const char * name) {

    if(ColorList.find(name) == ColorList.end()){
        std::cerr << " --- Error in the loading of the color palette: " << name << std::endl;
        return false;
    }
    else {
        // std::cout << " --- Loading the palette: " << name << std::endl;
        std::string n(name);
        std::string filename = ColorList[n];
        bool verbose_parser = false;
        ConfigParser conf(filename.c_str(),verbose_parser);
        Int_t NRGBs = conf.GetInt("NRGBs");
        Double_t * stops    = conf.GetDoubleArray("stops");
        Double_t * green    = conf.GetDoubleArray("green");
        Double_t * red      = conf.GetDoubleArray("red");
        Double_t * blue     = conf.GetDoubleArray("blue");
        _NRGBs   =   NRGBs;
        _stops   =   stops;
        _red     =   red;
        _green   =   green;
        _blue    =   blue;
        return true;
    }
}

/**
 * Class PlotUtils implementation
 */

PlotUtils::PlotUtils(): TObject(), _verbose(false), _rooplots(), _hist(), _graph(), _palette(), _canvas() {}

PlotUtils::PlotUtils(bool verbose): TObject(), _verbose(verbose), _rooplots(), _hist(), _graph(), _palette(), _canvas() {}

PlotUtils::PlotUtils(RooPlot * plot,const char * titleplot,
    const char * xlabel, const char * ylabel): TObject(), _verbose(false), _hist(), _graph(), _palette(), _canvas(){
    cout << " --- Initialising PlotUtils object" << endl;
    AddRooPlot(plot,titleplot,xlabel,ylabel);
}

PlotUtils::PlotUtils(TH1 * hist,const char * titlehist,
    const char * xlabel, const char * ylabel): TObject(), _verbose(false), _rooplots(),
    _hist(), _graph(), _palette(), _canvas(){
    cout << " --- Initialising PlotUtils object" << endl;
    AddHist(hist,titlehist,xlabel,ylabel);
}

PlotUtils::~PlotUtils()  {
    _rooplots.clear();
    _hist.clear();
    _graph.clear();
    delete _canvas;
    _canvas=0;
}

void PlotUtils::Clear() {
    vector<RooPlot*> rooplots;
    vector<TH1*> hist;
    vector<TGraphErrors*> graph;
    _rooplots = rooplots;
    _hist     = hist;
    _graph    = graph;
    if (_canvas != 0) delete _canvas;
    _canvas = 0;
}

void PlotUtils::AddRooPlot(RooPlot * plot, const char * title, const char * xlabel,
    const char * ylabel) {
    if(_verbose) cout << " --- Adding the RooPlot variable " << plot->GetTitle() << endl;
    plot->SetTitle(title);
    if(_verbose) cout << " --- Setting the title " << title << endl;
    if(xlabel != 0) {
        plot->SetXTitle(xlabel);
        if(_verbose) cout << " --- Setting the x label " << xlabel << endl;
    }
    if(ylabel != 0) {
        if(_verbose) cout << " --- Setting the y label " << ylabel << endl;
        plot->SetYTitle(ylabel);
    }
    _rooplots.push_back(plot);
}

void PlotUtils::AddHist(TH1 * hist, const char * titlehist,
                const char * xlabel,const char * ylabel) {

    if(_verbose) cout << " --- Adding the Histogram " << hist->GetTitle() << endl;
    hist->SetTitle(titlehist);
    hist->SetStats(0);
    if(_verbose) cout << " --- Setting the title " << titlehist << endl;
    if(xlabel != 0) {
        hist->SetXTitle(xlabel);
        if(_verbose) cout << " --- Setting the x label " << xlabel << endl;
    }
    if(ylabel != 0) {
        if(_verbose) cout << " --- Setting the y label " << ylabel << endl;
        hist->SetYTitle(ylabel);
    }
    _hist.push_back(hist);
}

void PlotUtils::AddGraph(TGraphErrors * graph, const char * titlegraph,
                const char * xlabel,const char * ylabel) {

    if(_verbose) cout << " --- Adding the Histogram " << graph->GetTitle() << endl;
    graph->SetTitle(titlegraph);
    if(_verbose) cout << " --- Setting the title " << titlegraph << endl;
    if(xlabel != 0) {
        graph->GetXaxis()->SetTitle(xlabel);
        if(_verbose) cout << " --- Setting the x label " << xlabel << endl;
    }
    if(ylabel != 0) {
        if(_verbose) cout << " --- Setting the y label " << ylabel << endl;
        graph->GetYaxis()->SetTitle(ylabel);
    }
    _graph.push_back(graph);
}

void PlotUtils::AddData(Double_t xleft, Double_t ydown, Double_t xright, Double_t ytop,
    const char * var1, const char * var2, const char * var3, const char * var4,
    const char * var5, const char * var6, const char * var7, const char * var8) {
        if(_verbose) cout << " --- Adding the text box to the plot" << endl;
        vector<const char*> vars;
        vars.push_back(var1);
        vars.push_back(var2);
        vars.push_back(var3);
        vars.push_back(var4);
        vars.push_back(var5);
        vars.push_back(var6);
        vars.push_back(var7);
        vars.push_back(var8);

        size_t imax(1);
        for(size_t i(1);i<vars.size();++i) {
            if(vars[i] == 0){
                    imax=i;
                    break;
            }
        }
        if(vars[vars.size()-1] != 0) {
            imax = vars.size();
        }

        TPaveText * box = new TPaveText(xleft,ydown,xright,ytop,"BRNDC");
        box->SetBorderSize(0); box->SetFillColor(0);
        box->SetTextAlign(12);//right aligned and vertically centered
        box->SetTextSize(0.03);
        for (size_t i = 0; i < imax; i++) {
            box->AddText(vars[i]);
            cout << " --- Adding the label: " << vars[i] << endl;
        }
        for(size_t i(0);i<_rooplots.size();++i) {
            _rooplots[i]->addObject(box);
            //plot->SetLabelSize(0.05);
        }
        for(size_t i(0);i<_hist.size();++i) {
            _hist[i]->GetListOfFunctions()->Add(box);
        }

}

//Operators
RooPlot& PlotUtils::operator[](size_t index) const {
     RooPlot* arg = _rooplots[index];
     if (index > _rooplots.size()) {
        cerr << " --- Index provided in PlotUtils::operator[] must be smaller than the number of plots included " << endl;
        RooPlot * ret = new RooPlot();
        return *ret;
    }
    return *arg ;
}

void PlotUtils::Sumw2(){
    for (size_t k = 0; k < _hist.size(); k++) {
        _hist[k]->Sumw2();
    }
}

void PlotUtils::SetStats(bool value) {
    for (size_t j = 0; j < _hist.size(); j++) {
        _hist[j]->SetStats(value);
    }
}

bool PlotUtils::SetPalette(const char * name, Int_t nContour) {
    ColorPalette palette;
    bool status = palette.LoadPalette(name);
    if (nContour > 0) _palette.SetNConts(nContour);
    if(_verbose) std::cout << " --- Number of contours "<< _palette.GetNConts() << std::endl;
    if (status){
        _palette = palette;
        TColor::CreateGradientColorTable(_palette.GetNRGBs(), _palette.GetStops(),
                                         _palette.GetRed(), _palette.GetGreen(),
                                         _palette.GetBlue(), _palette.GetNConts());

        for(size_t i(0); i<_hist.size();++i) {
            _hist[i]->SetContour(_palette.GetNConts());
        }
        return true;
    }
    else {
        return false;
    }
}

bool PlotUtils::SetLineColors(const char * name) {
    bool status = SetPalette(name);
    if (status){
        TColor::CreateGradientColorTable(_palette.GetNRGBs(), _palette.GetStops(),
                                         _palette.GetRed(), _palette.GetGreen(),
                                         _palette.GetBlue(), _palette.GetNConts());
        for (size_t j = 0; j < _hist.size(); j++) {
            Int_t color = TColor::GetColorPalette(_palette.GetNConts()/_hist.size()*j);
            _hist[j]->SetLineColor(color);
        }
        for (size_t j = 0; j < _graph.size(); j++) {
            Int_t color = TColor::GetColorPalette(_palette.GetNConts()/_hist.size()*j);
            _graph[j]->SetLineColor(color);
        }
        return true;
    }
    else {
        std::cerr << " --- Effor during the loading of palette: " << name << std::endl;
        return false;
    }
}

bool PlotUtils::SetLineColors(Color_t color){
    for (size_t j = 0; j < _hist.size(); j++) {
        _hist[j]->SetLineColor(color);
    }
    for (size_t j = 0; j < _graph.size(); j++) {
        _graph[j]->SetLineColor(color);
    }
    return true;
}

bool PlotUtils::SetMarkerColors(const char * name) {
    bool status = SetPalette(name);
    if (status){
        TColor::CreateGradientColorTable(_palette.GetNRGBs(), _palette.GetStops(),
                                         _palette.GetRed(), _palette.GetGreen(),
                                         _palette.GetBlue(), _palette.GetNConts());
        for (size_t j = 0; j < _hist.size(); j++) {
            Int_t color = TColor::GetColorPalette(_palette.GetNConts()/_hist.size()*j);
            _hist[j]->SetMarkerColor(color);
        }
        for (size_t j = 0; j < _graph.size(); j++) {
            Int_t color = TColor::GetColorPalette(_palette.GetNConts()/_graph.size()*j);
            _graph[j]->SetMarkerColor(color);
        }
        return true;
    }
    else {
        std::cerr << " --- Error during the loading of palette: " << name << std::endl;
        return false;
    }
}

bool PlotUtils::FillHistograms(const char * name, Float_t alpha) {
    if(_verbose) std::cout << " --- Filling the histograms" << std::endl;
    ColorPalette palette;
    bool status = palette.LoadPalette(name);
    if (status){
        _palette = palette;
        TColor::CreateGradientColorTable(_palette.GetNRGBs(), _palette.GetStops(),
                                         _palette.GetRed(), _palette.GetGreen(),
                                         _palette.GetBlue(), _palette.GetNConts(),alpha);

        for (size_t j = 0; j < _hist.size(); j++) {
            Int_t color = TColor::GetColorPalette(_palette.GetNConts()/_hist.size()*j);
            _hist[j]->SetFillColor(color);
            _hist[j]->SetLineColor(color);
            // Int_t transp = alpha * 100;
            // std::cout << "alpha:" << transp << std::endl;
            // _hist[j]->SetFillStyle(4000+transp); // Visible only in bitmap format
            // TColor::SetAlpha(alpha);
        }
        return true;
    }
    else return false;
}

void PlotUtils::SetMarkerStyles(Int_t marker_style){
    if (marker_style>=0) {
        for (size_t j = 0; j < _hist.size(); j++) {
            _hist[j]->SetMarkerStyle(marker_style);
            if(_verbose) std::cout << " --- Setting the marker style " << marker_styles[j]
            << endl;
        }
        for (size_t k = 0; k < _rooplots.size(); k++) {
            _rooplots[k]->SetMarkerStyle(marker_style);
            if(_verbose) std::cout << " --- Setting the marker style " << marker_styles[k]
            << endl;
        }
        for (size_t k = 0; k < _graph.size(); k++) {
            _graph[k]->SetMarkerStyle(marker_style);
            if(_verbose) std::cout << " --- Setting the marker style " << marker_styles[k]
            << endl;
        }
    }
    else {
        for (size_t j = 0; j < _hist.size(); j++) {
            _hist[j]->SetMarkerStyle(marker_styles[j]);
            if(_verbose) std::cout << " --- Setting the marker style " << marker_styles[j]
            << endl;
        }
        for (size_t k = 0; k < _rooplots.size(); k++) {
            _rooplots[k]->SetMarkerStyle(marker_styles[k]);
            if(_verbose) std::cout << " --- Setting the marker style " << marker_styles[k]
            << endl;
        }
        for (size_t k = 0; k < _graph.size(); k++) {
            _graph[k]->SetMarkerStyle(marker_styles[k]);
            if(_verbose) std::cout << " --- Setting the marker style " << marker_styles[k]
            << endl;
        }
    }
}

void PlotUtils::SetMarkerSize(Double_t scale){
    Float_t size = 0.;
    for (size_t j = 0; j < _hist.size(); j++) {
        size = _hist[j]->GetMarkerSize()*scale;
        _hist[j]->SetMarkerSize(size);
        if(_verbose) std::cout << " --- Setting the marker size " << scale
        << endl;
    }
    for (size_t k = 0; k < _graph.size(); k++) {
        size = _graph[k]->GetMarkerSize()*scale;
        _graph[k]->SetMarkerSize(size);
        if(_verbose) std::cout << " --- Setting the marker size " << scale
        << endl;
    }
}

void PlotUtils::SetSameBinning(Int_t nbins){
    if (nbins>=0){
        if(_verbose) std::cout << " --- Setting number of bins " << nbins << std::endl;
        for (size_t k = 0; k < _hist.size(); k++) {
            _hist[k]->Rebin(nbins);
        }
    }
    else {
        size_t hist_max_width = _hist[0]->GetBinWidth(0);
        for (size_t k = 0; k < _hist.size(); k++) {
            if(hist_max_width <= _hist[k]->GetBinWidth(0)) hist_max_width = _hist[k]->GetBinWidth(0);
        }
        for (size_t k = 0; k < _hist.size(); k++) {
            if(abs(round(hist_max_width/_hist[k]->GetBinWidth(0)) - hist_max_width/_hist[k]->GetBinWidth(0)) > 0.01) {
                if(abs(round(hist_max_width/_hist[k]->GetBinWidth(0)*10) - hist_max_width/_hist[k]->GetBinWidth(0)*10) < 0.01){
                        Int_t hist_int = round(_hist[0]->GetBinWidth(0)*100);
                        Int_t hist_int_new = round(_hist[1]->GetBinWidth(0)*100);
                        Int_t LCM = boost::math::lcm(hist_int,hist_int_new);
                    for (size_t k = 2; k < _hist.size(); k++) {
                        hist_int_new = round(_hist[k]->GetBinWidth(0)*100);
                        LCM = boost::math::lcm(LCM,hist_int_new);
                    }
                    for (size_t k = 0; k < _hist.size(); k++) {
                        Int_t rebin = round(LCM / (_hist[k]->GetBinWidth(0)*100));
                        if(_verbose) std::cout << " --- Merging " << rebin << " bins for histogram " << _hist[k]->GetName() << std::endl;
                        _hist[k]->Rebin(rebin);
                    }
                    break;
                }
                else{
                    std::cerr << "WARNING: Impossible to provide a rebinning for a factor " << hist_max_width/_hist[k]->GetBinWidth(0) << std::endl;
                    continue;
                }
            }
            else if (round(hist_max_width/_hist[k]->GetBinWidth(0)) == 1) {
                continue;
            }
            else {
                Int_t rebin = round(hist_max_width/_hist[k]->GetBinWidth(0));
                if(rebin != 0) {
                    _hist[k]->Rebin();
                    if(_verbose) std::cout << " --- Rebinning for " << hist_max_width/_hist[k]->GetBinWidth(0) << " " << round(hist_max_width/_hist[k]->GetBinWidth(0)) << std::endl;
                }
            }
        }
    }
    // Sumw2();
    // if(_canvas!=0)_canvas->Update();
}

TPad * PlotUtils::GetRatioPlot(Int_t ref_index, const char * label, Double_t y_pos, Double_t height){
    if(_verbose) std::cout << " --- Producing ratio plot with reference histogram: " << _hist[ref_index]->GetName() << std::endl;
    // SetXSameRange();
    SetSameBinning(); //It's necessary to set the same ranges and binnings for producing ratio plots
    TPad * ratio_pad = new TPad("ratio_pad","ratio_pad",0,y_pos,1,y_pos+height);
    ratio_pad->SetTopMargin(0.);
    ratio_pad->SetBottomMargin(height);
    ratio_pad->Draw();
    ratio_pad->cd();
    Double_t scale_factor = (1-height)/height;
    for (size_t k = 0; k < _hist.size(); k++) {
        if(k!=size_t(ref_index)){
            TString hist_name(_hist[k]->GetName());
            hist_name += "_clone";
            TH1 * hist = (TH1*) _hist[k]->Clone(hist_name.Data());
            hist->GetXaxis()->SetRange(_hist[k]->GetXaxis()->GetFirst(),_hist[k]->GetXaxis()->GetLast());
            // hist->Sumw2();
            hist->Divide(_hist[ref_index]);
            if(_verbose) std::cout << " --- Dividing " << hist->GetName() << " by " << _hist[ref_index]->GetName() << std::endl;
            hist->SetMaximum(2.1);
            hist->SetMinimum(0.0);
            hist->SetTitle("");
            hist->GetYaxis()->CenterTitle();
            hist->GetYaxis()->SetNdivisions(8,true);
            hist->GetYaxis()->SetTitleSize(_hist[k]->GetYaxis()->GetTitleSize()*scale_factor/1.5);
            hist->GetYaxis()->SetTitleOffset(0.8);
            hist->GetYaxis()->SetLabelSize(_hist[k]->GetYaxis()->GetLabelSize()*scale_factor);
            hist->GetYaxis()->SetLabelOffset(_hist[k]->GetYaxis()->GetLabelOffset()*scale_factor);
            hist->GetXaxis()->SetTitleSize(_hist[k]->GetXaxis()->GetTitleSize()*scale_factor);
            hist->GetXaxis()->SetLabelSize(_hist[k]->GetXaxis()->GetLabelSize()*scale_factor);
            if(label!=0){
                hist->SetYTitle(label);
            }
            hist->Draw();
        }
    }
    // Central line
    Double_t bwidth = _hist[0]->GetXaxis()->GetBinWidth(0);
    // Double_t xmin = _hist[0]->GetXaxis()->GetFirst()*bwidth;
    // Double_t xmax = _hist[0]->GetXaxis()->GetLast()*bwidth;
    Double_t xmin = _hist[0]->GetBinCenter(_hist[0]->GetXaxis()->GetFirst())-bwidth/2.;
    Double_t xmax = _hist[0]->GetBinCenter(_hist[0]->GetXaxis()->GetLast()) +bwidth/2.;
    TLine * line = new TLine(xmin,1.0,xmax,1.0);
    // line->SetLineWidth(width);
    line->SetLineStyle(2);
    line->SetLineColor(1);
    line->Draw();
    return (TPad*)ratio_pad->Clone();
}

// void PlotUtils::SetOptimalBinWidth(Double_t value){
//     for (size_t j = 0; j < _hist.size(); j++) {
//       if(_verbose) std::cout << " --- Setting optimal binning histogram " << _hist[j]->GetName() << std::endl;
//
//     }
// }

TCanvas * PlotUtils::PlotAllHist(const char * option, const char * canvas_name) {
    if(_verbose) std::cout << " --- Plotting the total histogram canvas" << std::endl;
    int nPlotsPerLine=5;
    int pixelsPerPad=300;


    int histsize = (int) _hist.size();
    int nPadx = 1;
    int nPady = 0;
    histsize / nPlotsPerLine > 0 ? nPadx = nPlotsPerLine : nPadx = histsize;
    histsize / nPlotsPerLine > 0 ? nPady = histsize/nPlotsPerLine : nPady = 1;
    TCanvas * canvas = new TCanvas(canvas_name,canvas_name,nPadx*pixelsPerPad,nPady*pixelsPerPad);
    canvas->Divide(nPadx,nPady);
    for(int i(0);i< histsize;++i) {
        canvas->cd(i+1);
        _hist[i]->Draw(option);
    }
    _canvas=canvas;
    return canvas;
}

TCanvas * PlotUtils::PlotHistSameCanvas(const char * option, const char * canvas_name){
    if(_verbose) std::cout << " --- Plotting all the histograms in the canvas: " << canvas_name << std::endl;
    TCanvas * canvas = new TCanvas(canvas_name,canvas_name);

    size_t index_max = 0;
    Double_t hist_max = _hist[0]->GetMaximum();
    // std::cout << "max: "<< hist_max << std::endl;
    for (size_t j = 1; j < _hist.size(); j++) {
        if (_hist[j]->GetMaximum() >= hist_max) {
            hist_max = _hist[j]->GetMaximum();
            index_max = j;
        }
    }
    _hist[index_max]->Draw();
    for (size_t j = 0; j < _hist.size(); j++) {
        if(j != index_max) _hist[j]->Draw("same");
    }
    gPad->RedrawAxis(); //Redraw the axis ticks of canvas
    _canvas=canvas;
    return canvas;
}

TCanvas * PlotUtils::PlotNormalizedHistSameCanvas(const char * option, const char * canvas_name){
    if(_verbose) std::cout << " --- Plotting all the normalized histograms on the canvas: " << canvas_name << std::endl;
    TCanvas * canvas = new TCanvas(canvas_name,canvas_name);

    size_t index_max = 0;
    _hist[0]->Scale(1./_hist[0]->Integral());
    Double_t hist_max = _hist[0]->GetMaximum();
    // std::cout << "max: "<< hist_max << std::endl;
    for (size_t j = 1; j < _hist.size(); j++) {
        _hist[j]->Scale(1./_hist[j]->Integral());
        if (_hist[j]->GetMaximum() >= hist_max) {
            hist_max = _hist[j]->GetMaximum();
            index_max = j;
        }
    }
    const char * draw_opt = "SAME";
    if(strcmp(option,"") != 0) draw_opt = (std::string(option) + " " + std::string(draw_opt)).c_str();

    _hist[index_max]->DrawNormalized(option);
    for (size_t j = 0; j < _hist.size(); j++) {
        if(j != index_max) _hist[j]->DrawNormalized(draw_opt);
    }
    gPad->RedrawAxis(); //Redraw the axis ticks of canvas
    _canvas=canvas;
    return canvas;
}

TCanvas * PlotUtils::PlotGraphSameCanvas(const char * option, const char * canvas_name){
    if(_verbose) std::cout << " --- Plotting all the graphs on the canvas: " << canvas_name << std::endl;
    TCanvas * canvas = new TCanvas(canvas_name,canvas_name);

    size_t index_max = 0;
    Double_t graph_max = _graph[0]->GetYaxis()->GetXmax();
    // std::cout << "max: "<< graph_max << std::endl;
    for (size_t j = 1; j < _graph.size(); j++) {
        if (_graph[j]->GetYaxis()->GetXmax() >= graph_max) {
            graph_max = _graph[j]->GetYaxis()->GetXmax();
            index_max = j;
        }
    }
    TString draw_opt_max(" SAME");
    TString draw_opt(" SAME");
    if(strcmp(option,"") != 0) draw_opt_max = TString("A") + TString(option) + TString(draw_opt_max);
    if(strcmp(option,"") != 0) draw_opt = TString(option) + TString(draw_opt);

    _graph[index_max]->Draw(draw_opt_max);
    for (size_t j = 0; j < _graph.size(); j++) {
        if(j != index_max) _graph[j]->Draw(draw_opt);
    }
    gPad->RedrawAxis(); //Redraw the axis ticks of canvas
    _canvas=canvas;
    return canvas;
}

void PlotUtils::DrawVerticalLine(Double_t x_pos, Color_t color, Style_t style) {
    if(_verbose) std::cout << " --- Draw vertical line at x=" << x_pos << std::endl;
    _canvas->Update();
    // std::cout << "vars: " << _canvas->GetUymin() << " " << _canvas->GetUymax() << std::endl;
    TLine * line = new TLine(x_pos,_canvas->GetUymin(),x_pos,_canvas->GetUymax());
    // line->SetLineWidth(width);
    line->SetLineStyle(style);
    line->SetLineColor(color);
    line->Draw();
}

void PlotUtils::DrawHorizontalLine(Double_t y_pos, Color_t color, Style_t style) {
    if(_verbose) std::cout << " --- Draw horizontal line at y=" << y_pos << std::endl;
    _canvas->Update();
    // std::cout << "vars: " << _canvas->GetUymin() << " " << _canvas->GetUymax() << std::endl;
    TLine * line = new TLine(_canvas->GetUxmin(),y_pos,_canvas->GetUxmax(),y_pos);
    // line->SetLineWidth(width);
    line->SetLineStyle(style);
    line->SetLineColor(color);
    line->Draw();
}

void PlotUtils::SetTitle(const char * title) {
    for (size_t j = 0; j < _hist.size(); j++) {
        _hist[j]->SetTitle(title);
        if(_verbose) std::cout << " --- Setting title " << title << " to " << _hist[j]->GetName()
        << std::endl;
    }
    for (size_t k = 0; k < _rooplots.size(); k++) {
        _rooplots[k]->SetTitle(title);
        if(_verbose) std::cout << " --- Setting title " << title << " to " << _rooplots[k]->GetName()
        << std::endl;
    }
    for (size_t k = 0; k < _graph.size(); k++) {
        _graph[k]->SetTitle(title);
        if(_verbose) std::cout << " --- Setting title " << title << " to " << _graph[k]->GetName()
        << std::endl;
    }
}

void PlotUtils::SetXTitle(const char * xlabel) {
    for (size_t j = 0; j < _hist.size(); j++) {
        _hist[j]->SetXTitle(xlabel);
        if(_verbose) std::cout << " --- Setting the x label " << xlabel << " to " << _hist[j]->GetName()
        << endl;
    }
    for (size_t k = 0; k < _rooplots.size(); k++) {
        _rooplots[k]->SetXTitle(xlabel);
        if(_verbose) std::cout << " --- Setting the x label " << xlabel << " to " << _rooplots[k]->GetName()
        << endl;
    }
    for (size_t k = 0; k < _graph.size(); k++) {
        _graph[k]->GetXaxis()->SetTitle(xlabel);
        if(_verbose) std::cout << " --- Setting the x label " << xlabel << " to " << _graph[k]->GetName()
        << endl;
    }
}

void PlotUtils::SetXTitleSize(Float_t size) {
    for (size_t j = 0; j < _hist.size(); j++) {
        _hist[j]->SetTitleSize(size,"X");
        if(_verbose) std::cout << " --- Setting " << _hist[j]->GetName() << " x title size to "
        << size << std::endl;
    }
    for (size_t j = 0; j < _rooplots.size(); j++) {
        _rooplots[j]->SetTitleSize(size,"X");
        if(_verbose) std::cout << " --- Setting " << _rooplots[j]->GetName() << " x title size to "
        << size << std::endl;
    }
    for (size_t j = 0; j < _graph.size(); j++) {
        _graph[j]->GetXaxis()->SetTitleSize(size);
        if(_verbose) std::cout << " --- Setting " << _graph[j]->GetName() << " x title size to "
        << size << std::endl;
    }
}

void PlotUtils::SetXTitleOffset(Float_t offset) {
    for (size_t j = 0; j < _hist.size(); j++) {
        _hist[j]->SetTitleOffset(offset,"X");
        if(_verbose) std::cout << " --- Setting " << _hist[j]->GetName() << " x title offset to "
        << offset << std::endl;
    }
    for (size_t j = 0; j < _rooplots.size(); j++) {
        _rooplots[j]->SetTitleOffset(offset,"X");
        if(_verbose) std::cout << " --- Setting " << _rooplots[j]->GetName() << " x title offset to "
        << offset << std::endl;
    }
    for (size_t j = 0; j < _graph.size(); j++) {
        _graph[j]->GetXaxis()->SetTitleOffset(offset);
        if(_verbose) std::cout << " --- Setting " << _graph[j]->GetName() << " x title offset to "
        << offset << std::endl;
    }
}

void PlotUtils::SetXLabelSize(Float_t size) {
    for (size_t j = 0; j < _hist.size(); j++) {
        _hist[j]->SetLabelSize(size,"X");
        if(_verbose) std::cout << " --- Setting " << _hist[j]->GetName() << " x label size to "
        << size << std::endl;
    }
    for (size_t j = 0; j < _rooplots.size(); j++) {
        _rooplots[j]->SetLabelSize(size,"X");
        if(_verbose) std::cout << " --- Setting " << _rooplots[j]->GetName() << " x label size to "
        << size << std::endl;
    }
    for (size_t j = 0; j < _graph.size(); j++) {
        _graph[j]->GetXaxis()->SetLabelSize(size);
        if(_verbose) std::cout << " --- Setting " << _graph[j]->GetName() << " x label size to "
        << size << std::endl;
    }
}

void PlotUtils::SetXLabelOffset(Float_t offset) {
    for (size_t j = 0; j < _hist.size(); j++) {
        _hist[j]->SetLabelOffset(offset,"X");
        if(_verbose) std::cout << " --- Setting " << _hist[j]->GetName() << " x label offset to "
        << offset << std::endl;
    }
    for (size_t j = 0; j < _rooplots.size(); j++) {
        _rooplots[j]->SetLabelOffset(offset,"X");
        if(_verbose) std::cout << " --- Setting " << _rooplots[j]->GetName() << " x label offset to "
        << offset << std::endl;
    }
    for (size_t j = 0; j < _graph.size(); j++) {
        _graph[j]->GetXaxis()->SetLabelOffset(offset);
        if(_verbose) std::cout << " --- Setting " << _graph[j]->GetName() << " x label offset to "
        << offset << std::endl;
    }
}

void PlotUtils::SetXRange(Double_t xmin, Double_t xmax){
    if(_verbose) std::cout << " --- Setting x range user [" << xmin << ", " << xmax << "]"
    << std::endl;
    for (size_t j = 0; j < _hist.size(); j++) {
        _hist[j]->GetXaxis()->SetRangeUser(xmin,xmax);
        // _hist[j]->GetXaxis()->SetLimits(xmin,xmax);
    }
    for (size_t j = 0; j < _rooplots.size(); j++) {
        _rooplots[j]->GetXaxis()->SetRangeUser(xmin,xmax);
        // _rooplots[j]->GetXaxis()->SetLimits(xmin,xmax);
    }
    for (size_t j = 0; j < _graph.size(); j++) {
        _graph[j]->GetXaxis()->SetRangeUser(xmin,xmax);
        // _graph[j]->GetXaxis()->SetLimits(xmin,xmax);
    }
}

void PlotUtils::SetXSameRange(){
    Float_t xmin = _hist[0]->GetXaxis()->GetXmin();
    Float_t xmax = _hist[0]->GetXaxis()->GetXmax();
    for (size_t j = 0; j < _hist.size(); j++) {
        if (_hist[j]->GetXaxis()->GetXmin() < xmin) xmin = _hist[j]->GetXaxis()->GetXmin();
        if (_hist[j]->GetXaxis()->GetXmax() > xmax) xmax = _hist[j]->GetXaxis()->GetXmax();
    }
    if(_verbose) std::cout << " --- Setting the same ranges for histograms: ["
                   << xmin << ", " << xmax << "]" << std::endl;
    for (size_t j = 0; j < _hist.size(); j++) {
        _hist[j]->GetXaxis()->SetLimits(xmin,xmax);
    }
}

void PlotUtils::SetYTitle(const char * ylabel) {
    for (size_t j = 0; j < _hist.size(); j++) {
        _hist[j]->SetYTitle(ylabel);
        if(_verbose) cout << " --- Setting the y label " << ylabel << " to " << _hist[j]->GetName()
        << endl;
    }
    for (size_t k = 0; k < _rooplots.size(); k++) {
        _rooplots[k]->SetYTitle(ylabel);
        if(_verbose) std::cout << " --- Setting the y label " << ylabel << " to " << _rooplots[k]->GetName()
        << endl;
    }
    for (size_t k = 0; k < _graph.size(); k++) {
        _graph[k]->GetYaxis()->SetTitle(ylabel);
        if(_verbose) std::cout << " --- Setting the y label " << ylabel << " to " << _graph[k]->GetName()
        << endl;
    }
}

void PlotUtils::SetYTitleSize(Float_t size) {
    for (size_t j = 0; j < _hist.size(); j++) {
        _hist[j]->SetTitleSize(size,"Y");
        if(_verbose) std::cout << " --- Setting " << _hist[j]->GetName() << " y title size to "
        << size << std::endl;
    }
    for (size_t j = 0; j < _rooplots.size(); j++) {
        _rooplots[j]->SetTitleSize(size,"Y");
        if(_verbose) std::cout << " --- Setting " << _rooplots[j]->GetName() << " y title size to "
        << size << std::endl;
    }
    for (size_t j = 0; j < _graph.size(); j++) {
        _graph[j]->GetYaxis()->SetTitleSize(size);
        if(_verbose) std::cout << " --- Setting " << _graph[j]->GetName() << " y title size to "
        << size << std::endl;
    }
}

void PlotUtils::SetYTitleOffset(Float_t offset) {
    for (size_t j = 0; j < _hist.size(); j++) {
        _hist[j]->SetTitleOffset(offset,"Y");
        if(_verbose) std::cout << " --- Setting " << _hist[j]->GetName() << " y title offset to "
        << offset << std::endl;
    }
    for (size_t j = 0; j < _rooplots.size(); j++) {
        _rooplots[j]->SetTitleOffset(offset,"Y");
        if(_verbose) std::cout << " --- Setting " << _rooplots[j]->GetName() << " y title offset to "
        << offset << std::endl;
    }
    for (size_t j = 0; j < _graph.size(); j++) {
        _graph[j]->GetYaxis()->SetTitleOffset(offset);
        if(_verbose) std::cout << " --- Setting " << _graph[j]->GetName() << " y title offset to "
        << offset << std::endl;
    }
}

void PlotUtils::SetYLabelSize(Float_t size) {
    for (size_t j = 0; j < _hist.size(); j++) {
        _hist[j]->SetLabelSize(size,"Y");
        if(_verbose) std::cout << " --- Setting " << _hist[j]->GetName() << " y label size to "
        << size << std::endl;
    }
    for (size_t j = 0; j < _rooplots.size(); j++) {
        _rooplots[j]->SetLabelSize(size,"Y");
        if(_verbose) std::cout << " --- Setting " << _rooplots[j]->GetName() << " y label size to "
        << size << std::endl;
    }
    for (size_t j = 0; j < _graph.size(); j++) {
        _graph[j]->GetYaxis()->SetLabelSize(size);
        if(_verbose) std::cout << " --- Setting " << _graph[j]->GetName() << " y label size to "
        << size << std::endl;
    }
}

void PlotUtils::SetYLabelOffset(Float_t offset) {
    for (size_t j = 0; j < _hist.size(); j++) {
        _hist[j]->SetLabelOffset(offset,"Y");
        if(_verbose) std::cout << " --- Setting " << _hist[j]->GetName() << " y label offset to "
        << offset << std::endl;
    }
    for (size_t j = 0; j < _rooplots.size(); j++) {
        _rooplots[j]->SetLabelOffset(offset,"Y");
        if(_verbose) std::cout << " --- Setting " << _rooplots[j]->GetName() << " y label offset to "
        << offset << std::endl;
    }
    for (size_t j = 0; j < _graph.size(); j++) {
        _graph[j]->GetYaxis()->SetLabelOffset(offset);
        if(_verbose) std::cout << " --- Setting " << _graph[j]->GetName() << " y label offset to "
        << offset << std::endl;
    }
}

void PlotUtils::SetYRange(Double_t ymin, Double_t ymax){
    if(_verbose) std::cout << " --- Setting y range user [" << ymin << ", " << ymax << "]"
    << std::endl;
    for (size_t j = 0; j < _hist.size(); j++) {
        _hist[j]->GetYaxis()->SetRangeUser(ymin,ymax);
    }
    for (size_t j = 0; j < _rooplots.size(); j++) {
        _rooplots[j]->GetYaxis()->SetRangeUser(ymin,ymax);
    }
    for (size_t j = 0; j < _graph.size(); j++) {
        _graph[j]->GetYaxis()->SetRangeUser(ymin,ymax);
    }
}

void PlotUtils::SetYLabelBinLenght(const char * ylabel, const char * bins_units) {
    for (size_t j = 0; j < _hist.size(); j++) {
        Double_t bin_width= _hist[j]->GetXaxis()->GetBinWidth(0);
        std::string label(ylabel);
        if (bins_units == 0) label += " / "+tostr(bin_width);
        else label += " / ( "+tostr(bin_width) + " " + bins_units+ " )";
        _hist[j]->SetYTitle(label.c_str());
        if(_verbose) cout << " --- Setting the y label " << label << " to " << _hist[j]->GetName()
        << endl;
    }
}

void PlotUtils::AddLegend(Double_t xleft, Double_t ydown, Double_t xright, Double_t ytop, const char * opt) {
    if(_verbose) std::cout << " --- Adding a legend to the plot" << std::endl;
    if(_canvas!=0) _canvas->Update();
    TLegend * legend = new TLegend(xleft,ydown,xright,ytop);
    legend->SetBorderSize(0);
    legend->SetFillColor(0);
    // legend->SetFont(42);
    // legend->SetTextSize(0.);

    for (size_t j = 0; j < _hist.size(); j++) {
        if (_hist[j]->GetFillColor() != 0) {
        legend->AddEntry(_hist[j],_hist[j]->GetTitle(),"f");
        }
        else {
            legend->AddEntry(_hist[j],_hist[j]->GetTitle(),opt);
        }
    }

    for (size_t j = 0; j < _graph.size(); j++) {
        legend->AddEntry(_graph[j],_graph[j]->GetTitle(),opt);
    }

    legend->Draw();
    SetTitle("");
}

void PlotUtils::SetLogy() {
    if(_verbose) std::cout << " --- Setting y logarithmic scale." << std::endl;
    _canvas->SetLogy();
    _canvas->Update();
}

// Atlas style functions
#if USE_ATLAS_STYLE
void PlotUtils::SetATLASStyle(){
    SetAtlasStyle();
}

void PlotUtils::AddTextAtlas(Double_t xleft, Double_t ydown, const char * label_text, Float_t scale_size, Color_t color) {
    Double_t dist=0.07*scale_size;
    Float_t xshift=-scale_size*0.0;
    ATLASLabel(xleft+xshift,ydown+dist,label_text,color,scale_size*1.1);
    myText(xleft,ydown,color,"#sqrt{s} = 13 TeV, #scale[0.7]{#int} L dt = 3.2 fb^{-1}",scale_size);
}

void PlotUtils::AddTextLine(Double_t xleft, Double_t ydown, const char * text, Float_t scale_size, Color_t color) {
    myText(xleft,ydown,color,text,scale_size);
}

void PlotUtils::FillGraphs(){
    if(_verbose) std::cout << " --- Filling graphs from histograms." << std::endl;
    for (size_t j = 0; j < _hist.size(); j++) {
        _graph.push_back(TH1TOTGraph(_hist[j]));
    }
}

// TCanvas * PlotUtils::PlotNormalizedATLASSameCanvas(const char * canvas_name){
//     if(_verbose) std::cout << " --- Plotting all the normalized graphs in the canvas: " << canvas_name << std::endl;
//     TCanvas * canvas = new TCanvas(canvas_name,canvas_name);
//
//     size_t index_max = 0;
//     _hist[0]->Scale(1./_hist[0]->Integral());
//     Double_t hist_max = _hist[0]->GetMaximum();
//     // std::cout << "max: "<< hist_max << std::endl;
//     for (size_t j = 1; j < _hist.size(); j++) {
//         _hist[j]->Scale(1./_hist[j]->Integral());
//         if (_hist[j]->GetMaximum() >= hist_max) {
//             hist_max = _hist[j]->GetMaximum();
//             index_max = j;
//         }
//     }
//     FillGraphs();
//     SetMarkerStyles(); SetLineColors(1); SetMarkerColors("rainbow");
//     SetXTitleSize(0.05);SetYTitleSize(0.05);
//     const char * draw_opt_max = "P";
//     const char * draw_opt = "P SAME";
//
//     _hist[index_max]->Draw(draw_opt_max);
//     // _graph[index_max]->Draw("AP");
//     for (size_t j = 0; j < _hist.size(); j++) {
//         if(j != index_max) _hist[j]->Draw(draw_opt);
//     }
//     gPad->RedrawAxis(); //Redraw the axis ticks of canvas
//     _canvas=canvas;
//     return canvas;
// }

TCanvas * PlotUtils::PlotATLASSameCanvas(bool setlogy, Int_t ref_index, const char * ratio_label, const char * canvas_name){
    if(_verbose) std::cout << " --- Plotting all the normalized graphs in the canvas: " << canvas_name << std::endl;
    TCanvas * canvas = 0;
    TPad * plot_pad = 0;
    if(ref_index>=0) {
        canvas = new TCanvas(canvas_name,canvas_name,600,650);
        plot_pad = new TPad("plot_pad","plot_pad",0,0.3,1,1.0);
        plot_pad->SetBottomMargin(0.);
        plot_pad->Draw();
        plot_pad->cd();
        if (setlogy) plot_pad->SetLogy();
        _canvas=canvas;
    }
    else {
        canvas = new TCanvas(canvas_name,canvas_name);
        _canvas=canvas;
        SetSameBinning();
    }


    size_t index_max = 0;
    // _hist[0]->Scale(1./_hist[0]->Integral());
    // _hist[0]->Sumw2();
    Double_t hist_max = _hist[0]->GetMaximum();
    for (size_t j = 1; j < _hist.size(); j++) {
        // _hist[j]->Scale(1./_hist[j]->Integral());
        if (_hist[j]->GetMaximum() >= hist_max) {
            hist_max = _hist[j]->GetMaximum();
            index_max = j;
        }
    }
    SetMarkerStyles(); SetLineColors(1); SetMarkerColors("rainbow");
    SetXTitleSize(0.05);SetYTitleSize(0.05);
    const char * draw_opt = "SAME";

    _hist[index_max]->Draw();
    std::cout << "Max hist1:" << _hist[0]->GetMaximum() << endl;
    std::cout << "Max hist2:" << _hist[1]->GetMaximum() << endl;
    // _graph[index_max]->Draw("AP");
    for (size_t j = 0; j < _hist.size(); j++) {
        if(j != index_max) _hist[j]->Draw(draw_opt);
    }
    gPad->RedrawAxis(); //Redraw the axis ticks of canvas
    if(ref_index>=0){
        canvas->cd();
        GetRatioPlot(ref_index, ratio_label,0.0,0.3);
        plot_pad->cd();
    }
    else{
        if(setlogy) SetLogy();
    }
    return canvas;
}

TCanvas * PlotUtils::PlotNormalizedATLASSameCanvas(bool setlogy, Int_t ref_index, const char * ratio_label, bool optimize_y_range, const char * canvas_name){
    if(_verbose) std::cout << " --- Plotting all the normalized graphs in the canvas: " << canvas_name << std::endl;
    TCanvas * canvas = 0;
    TPad * plot_pad = 0;
    if(ref_index>=0) {
        canvas = new TCanvas(canvas_name,canvas_name,600,650);
        plot_pad = new TPad("plot_pad","plot_pad",0,0.3,1,1.0);
        plot_pad->SetBottomMargin(0.);
        plot_pad->Draw();
        plot_pad->cd();
        if (setlogy) plot_pad->SetLogy();
        _canvas=canvas;
    }
    else {
        canvas = new TCanvas(canvas_name,canvas_name);
        _canvas=canvas;
        SetSameBinning();
    }


    size_t index_max = 0;
    size_t index_min = 0;
    _hist[0]->Scale(1./_hist[0]->Integral());
    // _hist[0]->Sumw2();
    Double_t hist_max = _hist[0]->GetMaximum();
    Double_t hist_min = _hist[0]->GetMinimum();
    for (size_t j = 1; j < _hist.size(); j++) {
        _hist[j]->Scale(1./_hist[j]->Integral());
        // _hist[j]->Sumw2();
        if (_hist[j]->GetMaximum() >= hist_max) {
            hist_max = _hist[j]->GetMaximum();
            index_max = j;
        }
        if (_hist[j]->GetMinimum()<= hist_min) {
            hist_min = _hist[j]->GetMinimum();
            index_min = j;
        }
    }
    if(optimize_y_range) SetYRange(fabs(hist_min-0.0000001),hist_max+0.1);

    SetMarkerStyles(); SetLineColors(1); SetMarkerColors("rainbow");
    SetXTitleSize(0.05);SetYTitleSize(0.05);
    const char * draw_opt_max = "P";
    const char * draw_opt = "P SAME";

    _hist[index_max]->Draw(draw_opt_max);
    // _graph[index_max]->Draw("AP");
    for (size_t j = 0; j < _hist.size(); j++) {
        if(j != index_max) _hist[j]->Draw(draw_opt);
    }
    gPad->RedrawAxis(); //Redraw the axis ticks of canvas
    if(ref_index>=0){
        canvas->cd();
        GetRatioPlot(ref_index, ratio_label,0.0,0.3);
        plot_pad->cd();
    }
    else{
        if(setlogy) SetLogy();
    }
    return canvas;
}
#endif
