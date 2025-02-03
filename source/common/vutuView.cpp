#include "vutuView.h"

#include "madronalib.h"

#include "MLDialBasic.h"
#include "MLTextButtonBasic.h"
#include "MLResizer.h"
#include "MLTextLabelBasic.h"
#include "MLSVGImage.h"
#include "MLSVGButtonBasic.h"

#include "MLParameters.h"
#include "MLSerialization.h"

#include "vutuPartialsDisplay.h"
#include "sampleDisplay.h"

#include "vutuProcessor.h"

#include "../build/resources/vutu/resources.c"


ml::Rect smallDialRect{0, 0, 1.0, 1.0};
ml::Rect mediumDialRect{0, 0, 2.0, 1.5};
ml::Rect largeDialRect{0, 0, 3, 2};
float mediumDialSize{0.625f};
float largeDialSize{0.875f};
ml::Rect labelRect(0, 0, 3, 1.0);

VutuView::VutuView(TextFragment appName, size_t instanceNum) :
  AppView(appName, instanceNum),
  PlatformView(nullptr, nullptr, nullptr, 0, 0) // Initialize base class with required arguments
{
  // get names of other Actors we might communicate with
  _controllerName = TextFragment(appName, "controller", ml::textUtils::naturalNumberToText(instanceNum));
  // Remove _processorName reference since it's not declared
  
  // register ourself
  auto myName = TextFragment(appName, "view", ml::textUtils::naturalNumberToText(instanceNum));
  registerActor(myName, this);
  
  // Set initial size
  setSizeInGridUnits(kDefaultGridUnits);
  // Remove setMinSizeInGridUnits call since it doesn't exist
  setGridSizeDefault(kDefaultGridUnitSize);

  Actor::start();
  std::cout << "VutuView: " << appName << " " << instanceNum << "\n";
}

VutuView::~VutuView ()
{
}

void VutuView::layoutView(DrawContext dc)
{
  // Use getSizeInGridUnits() from the base class
  Vec2 gridDims = _view->getSizeInGridUnits();
  int gx = gridDims.x();
  int gy = gridDims.y();
  
  // set grid size of entire view, for background and other drawing
  _view->setProperty("grid_units_x", gx);
  _view->setProperty("grid_units_y", gy);
  
  if(!_view->_widgets.size()) return;
  
  const int bottomHeight{7};
  int bottomY = gy - bottomHeight;
  float dialsY1 = bottomY + 2;
  float dialsY2 = bottomY + 5;
  
  // left dials
  _view->_widgets["resolution"]->setRectProperty("bounds", alignCenterToPoint(largeDialRect, {2.0, dialsY1}));
  _view->_widgets["amp_floor"]->setRectProperty("bounds", alignCenterToPoint(largeDialRect, {3.5, dialsY2}));
  _view->_widgets["window_width"]->setRectProperty("bounds", alignCenterToPoint(largeDialRect, {5.0, dialsY1}));
  
  _view->_widgets["lo_cut"]->setRectProperty("bounds", alignCenterToPoint(largeDialRect, {6.5, dialsY2}));
  _view->_widgets["hi_cut"]->setRectProperty("bounds", alignCenterToPoint(largeDialRect, {8, dialsY1}));
  
  _view->_widgets["freq_drift"]->setRectProperty("bounds", alignCenterToPoint(largeDialRect, {9.5, dialsY2}));
  _view->_widgets["noise_width"]->setRectProperty("bounds", alignCenterToPoint(largeDialRect, {11, dialsY1}));
  
  // right dials
  _view->_widgets["fundamental"]->setRectProperty("bounds", alignCenterToPoint(mediumDialRect, {gx - 2.f, bottomY + 1.5f}));
  _view->_widgets["test_volume"]->setRectProperty("bounds", alignCenterToPoint(mediumDialRect, {gx - 2.f, bottomY + 3.5f}));
  _view->_widgets["output_volume"]->setRectProperty("bounds", alignCenterToPoint(mediumDialRect, {gx - 2.f, bottomY + 5.5f}));

  // dial labels
  auto positionLabelUnderDial = [&](Path dialName)
  {
    Path labelName (TextFragment(pathToText(dialName), "_label"));
    ml::Rect dialRect = _view->_widgets[dialName]->getRectProperty("bounds");
    _view->_backgroundWidgets[labelName]->setRectProperty
    ("bounds", alignTopCenterToPoint(labelRect, dialRect.bottomCenter() - Vec2(0, 0.5)));
  };
  for(auto dialName : {"resolution", "window_width", "amp_floor", "lo_cut", "hi_cut", "noise_width", "freq_drift", "fundamental", "test_volume", "output_volume"})
  {
    positionLabelUnderDial(dialName);
  }
  
  // info: whole width
  _view->_widgets["info"]->setRectProperty("bounds", ml::Rect(0, bottomY, gx, 1));
  
  // audio display widgets
  int bigWidth = gx;
  _view->_widgets["source"]->setRectProperty("bounds", ml::Rect(0, 0, bigWidth, 2));
  _view->_widgets["partials"]->setRectProperty("bounds", ml::Rect(0, 2, bigWidth, bottomY - 4));
  _view->_widgets["synth"]->setRectProperty("bounds", ml::Rect(0, bottomY - 2, bigWidth, 2));

  // buttons
  int centerX = gx/2;
  float buttonWidth = 4;
  float halfButtonWidth = buttonWidth/2.f;

  ml::Rect textButtonRect(0, 0, buttonWidth, 1);

  
  float buttonsY1 = bottomY + 2.5;
  float buttonsY2 = bottomY + 4.0;
  float buttonsY3 = bottomY + 5.5;
  
  float buttonsR = gx - 4 - halfButtonWidth;
  float buttonsX1 = gx - 4 - halfButtonWidth - buttonWidth*2;
  float buttonsX2 = gx - 4 - halfButtonWidth - buttonWidth*1;
  float buttonsX3 = gx - 4 - halfButtonWidth - buttonWidth*0;
  
  _view->_widgets["open"]->setRectProperty("bounds", alignCenterToPoint(textButtonRect, {buttonsX1, buttonsY1}));
  _view->_widgets["analyze"]->setRectProperty("bounds", alignCenterToPoint(textButtonRect, {buttonsX2, buttonsY1}));
  _view->_widgets["play_source"]->setRectProperty("bounds", alignCenterToPoint(textButtonRect, {buttonsX3, buttonsY1}));
    
  _view->_widgets["import"]->setRectProperty("bounds", alignCenterToPoint(textButtonRect, {buttonsX1, buttonsY2}));
  _view->_widgets["synthesize"]->setRectProperty("bounds", alignCenterToPoint(textButtonRect, {buttonsX2, buttonsY2}));
  _view->_widgets["export"]->setRectProperty("bounds", alignCenterToPoint(textButtonRect, {buttonsX3, buttonsY2}));

  _view->_widgets["play_synth"]->setRectProperty("bounds", alignCenterToPoint(textButtonRect, {buttonsX1, buttonsY3}));
  _view->_widgets["export_synth"]->setRectProperty("bounds", alignCenterToPoint(textButtonRect, {buttonsX2, buttonsY3}));
  
  // other labels
  ml::Rect otherLabelsRect(0, 0, 2, 1);
  float labelsR = buttonsX1 - halfButtonWidth -  0.25;
  _view->_backgroundWidgets["source_label"]->setRectProperty("bounds", alignMiddleRightToPoint(otherLabelsRect, {labelsR, buttonsY1}));
  _view->_backgroundWidgets["partials_label"]->setRectProperty("bounds", alignMiddleRightToPoint(otherLabelsRect, {labelsR, buttonsY2}));
  _view->_backgroundWidgets["resynth_label"]->setRectProperty("bounds", alignMiddleRightToPoint(otherLabelsRect, {labelsR, buttonsY3}));

  
  
  // resize all widgets
  forEach< Widget >
  (_view->_widgets, [&](Widget& w)
   {
    w.resize(dc);
   }
   );
}

void VutuView::initializeResources(NativeDrawContext* nvg)
{
  // initialize drawing properties before controls are made
  _drawingProperties.setProperty("mark", colorToMatrix({0.01, 1.00, 0.01, 1.0}));
  _drawingProperties.setProperty("background", colorToMatrix({0.01, 0.01, 0.01, 1.0}));
  _drawingProperties.setProperty("panel_bg", colorToMatrix({0.01, 0.01, 0.01, 1.0}));
  _drawingProperties.setProperty("common_stroke_width", 1/24.f);
  _drawingProperties.setProperty("partials", colorToMatrix({0.01, 1.00, 0.01, 1.0}));

  // DEBUG
  _drawingProperties.setProperty("draw_widget_bounds", false);
  _drawingProperties.setProperty("draw_widget_outlines", false);
  
  if (nvg)
  {
      // fonts
      _resources.fonts["d_din"] = std::make_unique< FontResource >(nvg, "MLVG_sans", resources::D_DIN_otf, resources::D_DIN_otf_size);
      _resources.fonts["d_din_oblique"] = std::make_unique< FontResource >(nvg, "MLVG_italic", resources::D_DIN_Italic_otf, resources::D_DIN_Italic_otf_size);
  }
}


void VutuView::makeWidgets(const ParameterDescriptionList& pdl)
{
  // add labels to background
  auto addControlLabel = [&](Path name, TextFragment t)
  {
    _view->_backgroundWidgets.add_unique< TextLabelBasic >(name, WithValues{
      { "bounds", rectToMatrix(labelRect) },
      { "h_align", "center" },
      { "v_align", "middle" },
      { "text", t },
      { "font", "d_din_oblique" },
      { "text_size", 0.50 },
      { "text_spacing", 0.0f }
    } );
  };
  addControlLabel("resolution_label", "resolution");
  addControlLabel("window_width_label", "window freq.");
  addControlLabel("amp_floor_label", "amp floor");
  addControlLabel("freq_drift_label", "freq. drift");
  addControlLabel("lo_cut_label", "lo cut");
  addControlLabel("hi_cut_label", "hi cut");
  addControlLabel("noise_width_label", "noise width");
  addControlLabel("fundamental_label", "fundamental");
  addControlLabel("test_volume_label", "fund. volume");
  addControlLabel("output_volume_label", "output volume");

  auto addOtherLabel = [&](Path name, TextFragment t)
  {
    _view->_backgroundWidgets.add_unique< TextLabelBasic >(name, WithValues{
      { "bounds", rectToMatrix(labelRect) },
      { "h_align", "right" },
      { "v_align", "middle" },
      { "text", t },
      { "font", "d_din_oblique" },
      { "text_size", 0.50 },
      { "text_spacing", 0.0f }
    } );
  };
  addOtherLabel("source_label", "source");
  addOtherLabel("partials_label", "partials");
  addOtherLabel("resynth_label", "resynth");

  
  // dials
  _view->_widgets.add_unique< DialBasic >("resolution", WithValues{
    {"size", largeDialSize },
    {"feature_scale", 2.0 },
    {"param", "resolution" }
  } );
  _view->_widgets.add_unique< DialBasic >("window_width", WithValues{
    {"size", largeDialSize },
    {"feature_scale", 2.0 },
    {"param", "window_width" }
  } );
  _view->_widgets.add_unique< DialBasic >("amp_floor", WithValues{
    {"size", largeDialSize },
    {"feature_scale", 2.0 },
    {"param", "amp_floor" }
  } );
  
  _view->_widgets.add_unique< DialBasic >("freq_drift", WithValues{
    {"size", largeDialSize },
    {"feature_scale", 2.0 },
    {"param", "freq_drift" }
  } );
   
  _view->_widgets.add_unique< DialBasic >("lo_cut", WithValues{
    {"size", largeDialSize },
    {"feature_scale", 2.0 },
    {"param", "lo_cut" }
  } );
  
  _view->_widgets.add_unique< DialBasic >("hi_cut", WithValues{
    {"size", largeDialSize },
    {"feature_scale", 2.0 },
    {"param", "hi_cut" }
  } );
  
  _view->_widgets.add_unique< DialBasic >("noise_width", WithValues{
    {"size", largeDialSize },
    {"feature_scale", 2.0 },
    {"param", "noise_width" }
  } );
  
  _view->_widgets.add_unique< DialBasic >("fundamental", WithValues{
    {"size", mediumDialSize },
    {"feature_scale", 2.0 },
    {"fine_drag_scale", 0.005f },
    {"param", "fundamental" }
  } );
  
  _view->_widgets.add_unique< DialBasic >("test_volume", WithValues{
    {"size", mediumDialSize },
    {"feature_scale", 2.0 },
    {"param", "test_volume" }
  } );
  _view->_widgets.add_unique< DialBasic >("output_volume", WithValues{
    {"size", mediumDialSize },
    {"feature_scale", 2.0 },
    {"param", "output_volume" }
  } );

  // buttons
  _view->_widgets.add_unique< TextButtonBasic >("open", WithValues{
    {"text", "open" },
    {"action", "open" }
  } );
  _view->_widgets.add_unique< TextButtonBasic >("analyze", WithValues{
    {"text", "analyze" },
    {"action", "analyze" }
  } );
  _view->_widgets.add_unique< TextButtonBasic >("play_source", WithValues{
    {"text", "play" },
    {"action", "toggle_play_source" }
  } );
  _view->_widgets.add_unique< TextButtonBasic >("synthesize", WithValues{
    {"text", "synthesize" },
    {"action", "synthesize" }
  } );
  _view->_widgets.add_unique< TextButtonBasic >("import", WithValues{
    {"text", "import" },
    {"action", "import" }
  } );
  _view->_widgets.add_unique< TextButtonBasic >("export", WithValues{
    {"text", "export .utu" },
    {"action", "export" }
  } );
  _view->_widgets.add_unique< TextButtonBasic >("play_synth", WithValues{
    {"text", "play" },
    {"action", "toggle_play_synth" }
  } );
  _view->_widgets.add_unique< TextButtonBasic >("export_synth", WithValues{
    {"text", "export .wav" },
    {"action", "export_synth" }
  } );

  // info label
  _view->_widgets.add_unique< TextLabelBasic >("info", WithValues{
    { "h_align", "center" },
    { "v_align", "middle" },
    { "text", "vutu" },
    { "font", "d_din_oblique" },
    { "text_size", 0.5 },
    { "text_spacing", 0.0f }
  } );
  
  // source
  _view->_widgets.add_unique< SampleDisplay >("source", WithValues{
    {"enable_interval", true },
    {"param", "analysis_interval" }
  } );
  
  // partials
  // note: see knowsParam() override where the widget requests multiple parameters.
  _view->_widgets.add_unique< VutuPartialsDisplay >("partials", WithValues{    
    {"param", "analysis_interval" }
  } );
  
  // synth (synthesized sample)
  _view->_widgets.add_unique< SampleDisplay >("synth", WithValues{
    //{"param", "analysis_interval" }
  } );
  

  // make all the above Widgets visible
  forEach< Widget >
  (_view->_widgets, [&](Widget& w)
   {
    w.setProperty("visible", true);
   }
   );
  
  // play buttons disabled until we have a sample
  _view->_widgets["play_source"]->setProperty("enabled", false);
  _view->_widgets["analyze"]->setProperty("enabled", false);
  _view->_widgets["export"]->setProperty("enabled", false);
  _view->_widgets["play_synth"]->setProperty("enabled", false);
  _view->_widgets["export_synth"]->setProperty("enabled", false);

  _setupWidgets(pdl);
}

void VutuView::onMessage(Message msg)
{
  if(head(msg.address) == "editor")
  {
    // we are the editor, so remove "editor" and handle message
    msg.address = tail(msg.address);
  }
  
  switch(hash(head(msg.address)))
  {
    case(hash("set_param")):
    {
      switch(hash(second(msg.address)))
      {
        default:
        {
          // no local parameter was found, set a plugin parameter
          
          // store param value in local tree.
          Path paramName = tail(msg.address);
          _params.setFromNormalizedValue(paramName, msg.value);
          
          // if the parameter change message is not from the controller,
          // forward it to the controller.
          if(!(msg.flags & kMsgFromController))
          {
            sendMessageToActor(_controllerName, msg);
          }
          
          // if the message comes from a Widget, we do send the parameter back
          // to other Widgets so they can synchronize. It's up to individual
          // Widgets to filter out duplicate values.
          _sendParameterMessageToWidgets(msg);
        }
        break;
      }
      break;
    }
    case(hash("do")):
    {
      switch(hash(second(msg.address)))
      {
        case(hash("set_source_data")):
        {
          // Store the blob value to prevent it from being destroyed
          auto blobValue = msg.value.getBlobValue();
          const void* blobPtr = blobValue.data();
          Sample* pSample = *static_cast<Sample* const*>(blobPtr);
          _view->_widgets["source"]->receiveNamedRawPointer("sample", pSample);
          
          break;
        }
          
        case(hash("set_partials_data")):
        {
          // Store the blob value to prevent it from being destroyed
          auto blobValue = msg.value.getBlobValue();
          const void* blobPtr = blobValue.data();
          VutuPartialsData* pPartials = *static_cast<VutuPartialsData* const*>(blobPtr);
          _view->_widgets["partials"]->receiveNamedRawPointer("partials", pPartials);
          
          break;
        }
          
        case(hash("set_synth_data")):
        {
          // Store the blob value to prevent it from being destroyed
          auto blobValue = msg.value.getBlobValue();
          const void* blobPtr = blobValue.data();
          Sample* pSample = *static_cast<Sample* const*>(blobPtr);
          _view->_widgets["synth"]->receiveNamedRawPointer("sample", pSample);
          
          break;
        }
          
        case(hash("set_source_duration")):
        {
          Message intervalStartMsg{"set_prop/interval_start", 0};
          Message intervalEndMsg{"set_prop/interval_end", msg.value.getFloatValue()};
          sendMessage(_view->_widgets["source"], intervalStartMsg);
          sendMessage(_view->_widgets["source"], intervalEndMsg);

          break;
        }

          
        default:
        {
          // if the message is not from the controller,
          // forward it to the controller.
          if(!(msg.flags & kMsgFromController))
          {
            sendMessageToActor(_controllerName, msg);
          }
          break;
        }
      }
      break;
    }
    default:
    {
      // try to forward the message to another receiver
      switch(hash(head(msg.address)))
      {
        case(hash("info")):
        {
          msg.address = tail(msg.address);
          sendMessage(_view->_widgets["info"], msg);
          break;
        }
        case(hash("controller")):
        {
          msg.address = tail(msg.address);
          sendMessageToActor(_controllerName, msg);
          break;
        }
        case(hash("widget")):
        {
          
          msg.address = tail(msg.address);
          auto widgetName = head(msg.address);
          msg.address = tail(msg.address);          
          sendMessage(_view->_widgets[widgetName], msg);
          break;
        }
        default:
        {
          // uncaught
          break;
        }
      }
      break;
    }
  }
}

void VutuView::setSizeInGridUnits(const Vec2& size)
{
  // Implementation of setSizeInGridUnits
}

void VutuView::createPlatformView(void* windowPtr, int flags)
{
  // Implementation of createPlatformView
}
