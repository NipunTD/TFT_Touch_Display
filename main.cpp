// #include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <lvgl.h>
#include <Touch_GT911.h>
#include "touch.h"
#define RED LV_COLOR_MAKE(255, 0, 0)  // Define red color
#define BLUE LV_COLOR_MAKE(0, 0, 255) // Define blue color

#define LV_COLOR_WIHTE LV_COLOR_MAKE(0xff, 0xff, 0xff)
#define LV_COLOR_BLACK LV_COLOR_MAKE(0x00, 0x00, 0x00)
#define GREEN LV_COLOR_MAKE(0x00, 0xff, 0x00)
#define LV_DISP_DEF_REFR_PERIOD 16 


Arduino_ESP32RGBPanel *bus = new Arduino_ESP32RGBPanel(
    39 /* CS */, 48 /* SCK */, 47 /* SDA */,
    18 /* DE */, 17 /* VSYNC */, 16 /* HSYNC */, 21 /* PCLK */,
    11 /* R0 */, 12 /* R1 */, 13 /* R2 */, 14 /* R3 */, 0 /* R4 */,
    8 /* G0 */, 20 /* G1 */, 3 /* G2 */, 46 /* G3 */, 9 /* G4 */, 10 /* G5 */,
    4 /* B0 */, 5 /* B1 */, 6 /* B2 */, 7 /* B3 */, 15 /* B4 */
);
Arduino_ST7701_RGBPanel *gfx = new Arduino_ST7701_RGBPanel(
    bus, GFX_NOT_DEFINED /* RST */, 0 /* rotation */,
    true /* IPS */, 480 /* width */, 480 /* height */,
    st7701_type1_init_operations, sizeof(st7701_type1_init_operations), true /* BGR */,
    10 /* hsync_front_porch */, 8 /* hsync_pulse_width */, 50 /* hsync_back_porch */,
    10 /* vsync_front_porch */, 8 /* vsync_pulse_width */, 20 /* vsync_back_porch */);

#define GFX_BL 38
static u8_t count = 0;
/* Change to your screen resolution */
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *disp_draw_buf;
static lv_color_t *disp_draw_buf1;
static lv_disp_drv_t disp_drv;
// String QR_CODE = "Wait For Connection!";
// bool isQrCode = false;
static lv_obj_t *settingBtn;
static lv_obj_t *label5;

static lv_obj_t *tabview;
static lv_obj_t *tab1;
static lv_obj_t *tab2;
static lv_obj_t *tab3;
static lv_obj_t *tab4;
static lv_obj_t *line_chart;

static lv_obj_t *barchart_btn;
static lv_obj_t *linechart_btn;

float powerarray[24] = {294659.594, 294702.499, 294739.444, 294778.544, 294816.216, 294855.983,
                        294894.258, 294959.572, 295042.372, 295105.437, 295166.279, 295225.925,
                        295286.898, 295363.486, 295418.144, 295501.98, 295611.647, 295707.715,
                        295793.342, 295883.339, 295971.016, 296059.265, 296127.911, 296191.618};
uint8_t i;

static void btn_event_cb(lv_event_t *e)
{
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *btn = lv_event_get_target(e);
   

  if (code == LV_EVENT_CLICKED)
  {

    if (btn ==linechart_btn)
    {

      lv_tabview_set_act(tabview, 3, LV_ANIM_ON);
      
    }
    else if (btn == barchart_btn)
    {
      lv_tabview_set_act(tabview,2, LV_ANIM_ON);
    }
    
  }
}

void TabSwipe()
{
  tabview = lv_tabview_create(lv_scr_act(), LV_DIR_LEFT, NULL); // Create a tab view with tabs on the left side

  // Add three tabs (pages) to the tabview
  lv_obj_t *tab1 = lv_tabview_add_tab(tabview, "Tab 1");
  lv_obj_t *tab2 = lv_tabview_add_tab(tabview, "Tab 2");
  lv_obj_t *tab3 = lv_tabview_add_tab(tabview, "barchart");
  // lv_obj_t *tab4 = lv_tabview_add_tab(tabview, "Tab 4");

  lv_obj_t *page1 = lv_label_create(tab1);
  lv_label_set_text(page1, "Page 1");

      // // Create a style for large text
    // static lv_style_t style_large_text;
    // lv_style_init(&style_large_text);
    // lv_style_set_text_font(&style_large_text, &lv_font_montserrat_20);
    // lv_style_set_text_color(&style_large_text, lv_color_white());

    // // Create and style the "E GRAVITY SOLUTIONS" label
    // static lv_obj_t *front = lv_label_create(tab1);
    // lv_obj_add_style(front, &style_large_text, 0);
    // lv_label_set_recolor(front, true);
    // lv_label_set_text(front, "#ff1303 E GRAVITY # #ff1303 Solutions #");
    // lv_obj_align(front, LV_ALIGN_TOP_MID, 0, 0); // Align to top-middle

  // Add content to Tab 2
  lv_obj_t *page2 = lv_label_create(tab2);
  lv_label_set_text(page2, "Page 2");

  settingBtn = lv_btn_create(tab2);
  lv_obj_set_size(settingBtn, 100, 100);
  lv_obj_align(settingBtn, LV_ALIGN_BOTTOM_LEFT, 0, 0);

  lv_obj_add_event_cb(settingBtn, btn_event_cb, LV_EVENT_ALL, NULL);
  lv_obj_t *label2 = lv_label_create(settingBtn); /*Add a label to the button*/
  lv_label_set_text(label2, LV_SYMBOL_SETTINGS);  /*Set the labels text*/
  lv_obj_set_size(label2, 50, 50);
  lv_obj_center(label2);

  static lv_obj_t *front = lv_label_create(tab1);
  lv_label_set_recolor(front, true);
  lv_label_set_text(front, "#ff1303 E GRAVITY # #ff1303 Solutions #");

  lv_obj_align(front, LV_ALIGN_CENTER, 0, 0);

  lv_obj_t *bar_chart = lv_chart_create(tab3);
  lv_obj_set_size(bar_chart, 430, 430); // Set size of the chart
  lv_chart_set_type(bar_chart, LV_CHART_TYPE_BAR);
  lv_obj_align(bar_chart, LV_ALIGN_CENTER, 10, -10); // Step 3: Set to bar chart

  // Step 4: Add a series to the bar chart
  lv_chart_series_t *series1 = lv_chart_add_series(bar_chart, GREEN, LV_CHART_AXIS_PRIMARY_Y);

  // Step 5: Set point count for both series
  lv_chart_set_point_count(bar_chart, 24); // Set the number of data points

  // Create the Y-axis label
  lv_obj_t *y_label = lv_label_create(tab3);
  // Set the text for the Y-axis
  lv_obj_align(y_label, LV_ALIGN_LEFT_MID, 0, 0); // Position to the left of the chart
  lv_label_set_text(y_label, "KWH");

  // Step 6: Populate the data points
  // Assuming you want to set some example values for the bars
  series1->y_points[0] = 10;
  series1->y_points[1] = 20;
  series1->y_points[2] = 15;
  series1->y_points[3] = 25;
  series1->y_points[4] = 30;

  // Optionally, set the axis range if necessary
  lv_chart_set_range(bar_chart, LV_CHART_AXIS_PRIMARY_Y, 0, 50);

  // Set the number of points (categories) on the X-axis
  lv_chart_set_point_count(bar_chart, 24); // Simulating 12 categories (e.g., months)

  // Refresh the chart to apply changes
  lv_chart_refresh(bar_chart);

  // Add content to Tab 3

  // Create the X-axis label
  lv_obj_t *x_label = lv_label_create(tab3);
  lv_label_set_text(x_label, "TIME(HOURS)");          // Set the text for the X-axis
  lv_obj_align(x_label, LV_ALIGN_BOTTOM_MID, 0, -20); // Position below the chart

  lv_chart_set_axis_tick(bar_chart, LV_CHART_AXIS_PRIMARY_X, 10, 5, 24, 10, true, 50);
  lv_chart_set_axis_tick(bar_chart, LV_CHART_AXIS_PRIMARY_Y, 10, 5, 10, 10, true, 50);

  lv_obj_t *tab4 = lv_tabview_add_tab(tabview, "Line_chart");

  lv_obj_t *line_chart = lv_chart_create(tab4);
  lv_obj_set_size(line_chart, 430, 430); // Set size of the chart
  lv_chart_set_type(line_chart, LV_CHART_TYPE_LINE);
  lv_obj_align(line_chart, LV_ALIGN_CENTER, 15, -10); // Step 3: Set to LINE CHART

  // Create the X-axis label
  lv_obj_t *x_label2 = lv_label_create(tab4);
  lv_label_set_text(x_label2, "TIME(HOURS)");          // Set the text for the X-axis
  lv_obj_align(x_label2, LV_ALIGN_BOTTOM_MID, 0, -20); // Position below the chart

  lv_chart_series_t *series2 = lv_chart_add_series(line_chart, RED, LV_CHART_AXIS_PRIMARY_Y);
  lv_chart_set_point_count(line_chart, 24);

  lv_chart_set_range(line_chart, LV_CHART_AXIS_PRIMARY_Y, 200, 400);
  for (i = 0; i <= 23; i++)
  {
    series2->y_points[i] = powerarray[i] / 1000;
    // series2->y_points[1] = 20;
    // series2->y_points[2] = 15;
    // series2->y_points[3] = 25;
    // series2->y_points[4] = 30;
  }

  lv_chart_refresh(line_chart);

  lv_chart_set_axis_tick(line_chart, LV_CHART_AXIS_PRIMARY_X, 10, 5, 24, 10, true, 50);
  lv_chart_set_axis_tick(line_chart, LV_CHART_AXIS_PRIMARY_Y, 10, 5, 10, 10, true, 50);

  barchart_btn = lv_btn_create(tab1);
  lv_obj_set_size(barchart_btn, 200, 200);
  lv_obj_align(barchart_btn, LV_ALIGN_BOTTOM_LEFT, 0, 0);
  

  lv_obj_add_event_cb(barchart_btn, btn_event_cb, LV_EVENT_ALL, tabview);
  lv_obj_t *barchart_label = lv_label_create(barchart_btn); /*Add a label to the button*/
  lv_label_set_text(barchart_label, "BARCHART");            /*Set the labels text*/
  lv_obj_center(barchart_label);

  linechart_btn = lv_btn_create(tab1);
  lv_obj_set_size(linechart_btn, 200, 200);
  lv_obj_align(linechart_btn, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
  

  lv_obj_add_event_cb(linechart_btn, btn_event_cb, LV_EVENT_ALL, tabview);
  lv_obj_t *linechart_label = lv_label_create(linechart_btn); /*Add a label to the button*/
  lv_label_set_text(linechart_label, "LINECHART");            /*Set the labels text*/
  lv_obj_center(linechart_label);
}

void display_wait(String message, String message1)
{
  /*Create a style for the shadow*/

  lv_obj_clean(lv_scr_act());

  // lv_obj_t *background = lv_obj_create(lv_scr_act());
  // lv_obj_set_size(background, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
  // lv_obj_set_style_bg_color(background, LV_COLOR_WIHTE, 0);

  lv_obj_t *label = lv_label_create(tab1);
  lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP); /*Break the long lines*/
  lv_label_set_recolor(label, true);                 /*Enable re-coloring by commands in the text*/
  lv_label_set_text(label, message.c_str());
  lv_obj_set_width(label, 400); /*Set smaller width to make the lines wrap*/

  lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

  // multiple objects
  lv_obj_t *label1 = lv_label_create(tab1);
  lv_label_set_text(label1, message1.c_str());
  lv_obj_align(label1, LV_ALIGN_CENTER, 0, 20); // Center of the screen
}

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{

  // gfx->fillScreen(0x0000);

  gfx->draw16bitRGBBitmap(area->x1 + 1,
                          area->y1 + 1,
                          (uint16_t *)&color_p->full,
                          (area->x2 - area->x1 + 1),
                          (area->y2 - area->y1 + 1));

  lv_disp_flush_ready(disp);
}

void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
  if (touch_has_signal())
  {
    if (touch_touched())
    {
      data->state = LV_INDEV_STATE_PR;

      /*Set the coordinates*/
      data->point.x = touch_last_x;
      data->point.y = touch_last_y;
    }
    else if (touch_released())
    {
      data->state = LV_INDEV_STATE_REL;
    }
  }
  else
  {
    data->state = LV_INDEV_STATE_REL;
  }
}

void setup()
{
  Serial.begin(115200);
  // Init Display
  gfx->begin(12000000);

  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);

  touch_init();

  gfx->fillScreen(BLACK);

  lv_init();
  disp_draw_buf = (lv_color_t *)malloc(sizeof(lv_color_t) * ((gfx->width() * 200) + 1));
  if (!disp_draw_buf)
  {
    disp_draw_buf1 = (lv_color_t *)malloc(sizeof(lv_color_t) * ((gfx->width() * 200) + 1));
    if (!disp_draw_buf)
      return;
  }
  lv_disp_draw_buf_init(&draw_buf, disp_draw_buf,disp_draw_buf1, (gfx->width() * 200) + 1);
  // /* Initialize the display */
  lv_disp_drv_init(&disp_drv);
  /* Change the following line to your display resolution */
  disp_drv.hor_res = gfx->width();
  disp_drv.ver_res = gfx->height();
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  String message = "#000000 ******** # #ff1303 E GRAVITY # #ff1303 Solutions # #000000 ******** #";
  String message1 = "pannipitiya";
  display_wait(message, message1);

  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);          // Initialize the input device driver
  indev_drv.type = LV_INDEV_TYPE_POINTER; // Define the input type
  indev_drv.read_cb = my_touchpad_read;   // Assign the read function
  lv_indev_drv_register(&indev_drv);      // Register the input device

  TabSwipe();
}

void loop()
{

  lv_timer_handler(); /* let the GUI do its work */
}
