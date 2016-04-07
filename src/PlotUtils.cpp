#include <map>

#include "PlotUtils.h"

#include "TPaveText.h"
#include "TText.h"
#include "TColor.h"
#include "ConfigParser.h"
#include "TAttFill.h"
#include "TLine.h"
#include "TLegend.h"

using namespace RooFit;

std::map<std::string,std::string> ColorList = {
    {"rainbow","/home/mvalente/script/config/plotconfig/rainbow.conf"},
    {"kbird","/home/mvalente/script/config/plotconfig/kbird.conf"}
};

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
        std::cout << " --- Error in the loading of the color palette: " << name << std::endl;
        return false;
    }
    else {
        std::cout << " --- Loading the palette: " << name << std::endl;
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

PlotUtils::PlotUtils(): TObject(), _rooplots(), _hist(), _palette(),_canvas() {}

PlotUtils::PlotUtils(RooPlot * plot,const char * titleplot,
    const char * xlabel, const char * ylabel): TObject(), _hist(), _palette(), _canvas(){
    cout << " --- Initialising PlotUtils object" << endl;
    AddRooPlot(plot,titleplot,xlabel,ylabel);
}

PlotUtils::PlotUtils(TH1 * hist,const char * titlehist,
    const char * xlabel, const char * ylabel): TObject(), _rooplots(), _hist(),
    _palette(),_canvas(){
    cout << " --- Initialising PlotUtils object" << endl;
    AddHist(hist,titlehist,xlabel,ylabel);
}

void PlotUtils::AddRooPlot(RooPlot * plot, const char * title, const char * xlabel,
    const char * ylabel) {
    cout << " --- Adding the RooPlot variable " << plot->GetTitle() << endl;
    plot->SetTitle(title);
    cout << " --- Setting the title " << title << endl;
    if(xlabel != 0) {
        plot->SetXTitle(xlabel);
        cout << " --- Setting the x label " << xlabel << endl;
    }
    if(ylabel != 0) {
        cout << " --- Setting the y label " << ylabel << endl;
        plot->SetYTitle(ylabel);
    }
    _rooplots.push_back(plot);
}


void PlotUtils::AddHist(TH1 * hist, const char * titlehist,
                const char * xlabel,const char * ylabel) {

    cout << " --- Adding the Histogram " << hist->GetTitle() << endl;
    hist->SetTitle(titlehist);
    hist->SetStats(0);
    cout << " --- Setting the title " << titlehist << endl;
    if(xlabel != 0) {
        hist->SetXTitle(xlabel);
        cout << " --- Setting the x label " << xlabel << endl;
    }
    if(ylabel != 0) {
        cout << " --- Setting the y label " << ylabel << endl;
        hist->SetYTitle(ylabel);
    }
    _hist.push_back(hist);
}

void PlotUtils::AddTextLHCb(Int_t year, Double_t xleft, Double_t ydown) {
    cout << " --- Adding the LHCb box data" << endl;
    TPaveText * label = new TPaveText (xleft,ydown,xleft+0.1,ydown+0.16,"BRNDC");
    label->SetBorderSize(0); label->SetFillColor(0);
    label->SetTextAlign(12); //right aligned and vertically centered
    label->SetTextSize (0.04);
    label->AddText("LHC#font[12]{b} unofficial");

    if(year == 2011) {
        label->AddText("#sqrt{s} = 7 TeV");
        label->AddText("#scale[0.6]{#int} L dt = 1 fb^{-1}");
    }
    else if (year == 2012) {
        label->AddText("#sqrt{s} = 8 TeV");
        label->AddText("#scale[0.6]{#int} L dt = 2 fb^{-1}");
    }
    else if(year == 4023) {
    label->AddText("#sqrt{s} = 7-8 TeV");
    label->AddText("#scale[0.6]{#int} L dt = 3 fb^{-1}");
    }
    for(size_t i(0);i<_rooplots.size();++i) {
        _rooplots[i]->addObject(label);
        //plot->SetLabelSize(0.05);
    }
    for (size_t j = 0; j < _hist.size(); j++) {
        _hist[j]->GetListOfFunctions()->Add(label);
    }
}

void PlotUtils::AddData(Double_t xleft, Double_t ydown, Double_t xright, Double_t ytop,
    const char * var1, const char * var2, const char * var3, const char * var4,
    const char * var5, const char * var6, const char * var7, const char * var8) {
        cout << " --- Adding the text box to the plot" << endl;
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

void PlotUtils::SetStats(bool value) {
    for (size_t j = 0; j < _hist.size(); j++) {
        _hist[j]->SetStats(value);
    }
}


bool PlotUtils::SetPalette(const char * name, Int_t nContour) {
    ColorPalette palette;
    bool status = palette.LoadPalette(name);
    if (nContour > 0) _palette.SetNConts(nContour);
    std::cout << " --- Number of contours "<< _palette.GetNConts() << std::endl;
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

bool PlotUtils::FillHistograms(const char * name, Float_t alpha) {
    std::cout << " --- Filling the histograms" << std::endl;
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

TCanvas * PlotUtils::PlotAllHist(const char * option, const char * canvas_name) {
    std::cout << " --- Plotting the total histogram canvas" << std::endl;
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
    std::cout << " --- Plotting all the histograms in the canvas: " << canvas_name << std::endl;
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

void PlotUtils::Reset() {
    vector<RooPlot*> rooplots;
    vector<TH1*> hist;
    _rooplots = rooplots;
    _hist = hist;
    _canvas = 0;
}

void PlotUtils::DrawVerticalLine(Double_t x_pos, Color_t color, Style_t style) {
    std::cout << " --- Draw vertical line at x=" << x_pos << std::endl;
    _canvas->Update();
    // std::cout << "vars: " << _canvas->GetUymin() << " " << _canvas->GetUymax() << std::endl;
    TLine * line = new TLine(x_pos,_canvas->GetUymin(),x_pos,_canvas->GetUymax());
    // line->SetLineWidth(width);
    line->SetLineStyle(style);
    line->SetLineColor(color);
    line->Draw();
}

void PlotUtils::DrawHorizontalLine(Double_t y_pos, Color_t color, Style_t style) {
    std::cout << " --- Draw horizontal line at y=" << y_pos << std::endl;
    _canvas->Update();
    // std::cout << "vars: " << _canvas->GetUymin() << " " << _canvas->GetUymax() << std::endl;
    TLine * line = new TLine(_canvas->GetUxmin(),y_pos,_canvas->GetUxmax(),y_pos);
    // line->SetLineWidth(width);
    line->SetLineStyle(style);
    line->SetLineColor(color);
    line->Draw();
}
/*
hist->SetTitle(titlehist);
hist->SetStats(0);
cout << " --- Setting the title " << titlehist << endl;
if(xlabel != 0) {
hist->SetXTitle(xlabel);
cout << " --- Setting the x label " << xlabel << endl;
}
if(ylabel != 0) {
cout << " --- Setting the y label " << ylabel << endl;
hist->SetYTitle(ylabel);
}
_hist.push_back(hist);
}
*/
void PlotUtils::SetTitle(const char * title) {
    for (size_t j = 0; j < _hist.size(); j++) {
        _hist[j]->SetTitle(title);
        std::cout << " --- Setting title " << title << " to " << _hist[j]->GetName()
        << std::endl;
    }
    for (size_t k = 0; k < _rooplots.size(); k++) {
        _rooplots[k]->SetTitle(title);
        std::cout << " --- Setting title " << title << " to " << _rooplots[k]->GetName()
        << std::endl;
    }
}

void PlotUtils::SetXTitle(const char * xlabel) {
    for (size_t j = 0; j < _hist.size(); j++) {
        _hist[j]->SetXTitle(xlabel);
        std::cout << " --- Setting the x label " << xlabel << " to " << _hist[j]->GetName()
        << endl;
    }
    for (size_t k = 0; k < _rooplots.size(); k++) {
        _rooplots[k]->SetYTitle(xlabel);
        std::cout << " --- Setting the x label " << xlabel << " to " << _rooplots[k]->GetName()
        << endl;
    }
}

void PlotUtils::SetXTitleSize(Float_t size) {
    for (size_t j = 0; j < _hist.size(); j++) {
        _hist[j]->SetTitleSize(size,"X");
        std::cout << " --- Setting " << _hist[j]->GetName() << " x title size to "
        << size << std::endl;
    }
    for (size_t j = 0; j < _rooplots.size(); j++) {
        _rooplots[j]->SetTitleSize(size,"X");
        std::cout << " --- Setting " << _rooplots[j]->GetName() << " x title size to "
        << size << std::endl;
    }
}

void PlotUtils::SetXTitleOffset(Float_t offset) {
    for (size_t j = 0; j < _hist.size(); j++) {
        _hist[j]->SetTitleOffset(offset,"X");
        std::cout << " --- Setting " << _hist[j]->GetName() << " x title offset to "
        << offset << std::endl;
    }
    for (size_t j = 0; j < _rooplots.size(); j++) {
        _rooplots[j]->SetTitleOffset(offset,"X");
        std::cout << " --- Setting " << _rooplots[j]->GetName() << " x title offset to "
        << offset << std::endl;
    }
}

void PlotUtils::SetXLabelSize(Float_t size) {
    for (size_t j = 0; j < _hist.size(); j++) {
        _hist[j]->SetLabelSize(size,"X");
        std::cout << " --- Setting " << _hist[j]->GetName() << " x label size to "
        << size << std::endl;
    }
    for (size_t j = 0; j < _rooplots.size(); j++) {
        _rooplots[j]->SetLabelSize(size,"X");
        std::cout << " --- Setting " << _rooplots[j]->GetName() << " x label size to "
        << size << std::endl;
    }
}

void PlotUtils::SetXLabelOffset(Float_t offset) {
    for (size_t j = 0; j < _hist.size(); j++) {
        _hist[j]->SetLabelOffset(offset,"X");
        std::cout << " --- Setting " << _hist[j]->GetName() << " x label offset to "
        << offset << std::endl;
    }
    for (size_t j = 0; j < _rooplots.size(); j++) {
        _rooplots[j]->SetLabelOffset(offset,"X");
        std::cout << " --- Setting " << _rooplots[j]->GetName() << " x label offset to "
        << offset << std::endl;
    }
}

void PlotUtils::SetYTitle(const char * ylabel) {
    for (size_t j = 0; j < _hist.size(); j++) {
        _hist[j]->SetYTitle(ylabel);
        cout << " --- Setting the y label " << ylabel << " to " << _hist[j]->GetName()
        << endl;
    }
    for (size_t k = 0; k < _rooplots.size(); k++) {
        _rooplots[k]->SetYTitle(ylabel);
        std::cout << " --- Setting the y label " << ylabel << " to " << _rooplots[k]->GetName()
        << endl;
    }
}

void PlotUtils::SetYTitleSize(Float_t size) {
    for (size_t j = 0; j < _hist.size(); j++) {
        _hist[j]->SetTitleSize(size,"Y");
        std::cout << " --- Setting " << _hist[j]->GetName() << " y title size to "
        << size << std::endl;
    }
    for (size_t j = 0; j < _rooplots.size(); j++) {
        _rooplots[j]->SetTitleSize(size,"Y");
        std::cout << " --- Setting " << _rooplots[j]->GetName() << " y title size to "
        << size << std::endl;
    }
}

void PlotUtils::SetYTitleOffset(Float_t offset) {
    for (size_t j = 0; j < _hist.size(); j++) {
        _hist[j]->SetTitleOffset(offset,"Y");
        std::cout << " --- Setting " << _hist[j]->GetName() << " y title offset to "
        << offset << std::endl;
    }
    for (size_t j = 0; j < _rooplots.size(); j++) {
        _rooplots[j]->SetTitleOffset(offset,"Y");
        std::cout << " --- Setting " << _rooplots[j]->GetName() << " y title offset to "
        << offset << std::endl;
    }
}

void PlotUtils::SetYLabelSize(Float_t size) {
    for (size_t j = 0; j < _hist.size(); j++) {
        _hist[j]->SetLabelSize(size,"Y");
        std::cout << " --- Setting " << _hist[j]->GetName() << " y label size to "
        << size << std::endl;
    }
    for (size_t j = 0; j < _rooplots.size(); j++) {
        _rooplots[j]->SetLabelSize(size,"Y");
        std::cout << " --- Setting " << _rooplots[j]->GetName() << " y label size to "
        << size << std::endl;
    }
}

void PlotUtils::SetYLabelOffset(Float_t offset) {
    for (size_t j = 0; j < _hist.size(); j++) {
        _hist[j]->SetLabelOffset(offset,"Y");
        std::cout << " --- Setting " << _hist[j]->GetName() << " y label offset to "
        << offset << std::endl;
    }
    for (size_t j = 0; j < _rooplots.size(); j++) {
        _rooplots[j]->SetLabelOffset(offset,"Y");
        std::cout << " --- Setting " << _rooplots[j]->GetName() << " y label offset to "
        << offset << std::endl;
    }
}

void PlotUtils::SetYLabelBinLenght(const char * ylabel, const char * bins_units) {
    for (size_t j = 0; j < _hist.size(); j++) {
        Double_t bin_width= _hist[j]->GetXaxis()->GetBinWidth(0);
        std::string label(ylabel);
        if (bins_units == 0) label += " / "+tostr(bin_width);
        else label += " / ( "+tostr(bin_width) + " " + bins_units+ " )";
        _hist[j]->SetYTitle(label.c_str());
        cout << " --- Setting the y label " << label << " to " << _hist[j]->GetName()
        << endl;
    }
}

void PlotUtils::AddLegend(Double_t xleft, Double_t ydown, Double_t xright, Double_t ytop) {
    std::cout << " --- Adding a legend to the plot" << std::endl;
    _canvas->Update();
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
            legend->AddEntry(_hist[j],_hist[j]->GetTitle(),"l");
        }
    }
    legend->Draw();

}
