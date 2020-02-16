#include <math.h>
#include <pango/pangocairo.h>
#include <cairo-pdf.h>

static void
draw_header(cairo_t *cr, char *label)
{
    PangoLayout *layout;
    PangoFontDescription *desc;
    PangoAttrList *attr_list;
    char *todraw;

    layout = pango_cairo_create_layout(cr);
    pango_layout_set_width(layout, 150 * PANGO_SCALE);

    desc = pango_font_description_from_string("Sans Italic 6");
    cairo_translate(cr, 20, 20);
    pango_parse_markup(
        "Si tu veux aller <b>vite</b>, marche <b>seul</b>\nSi tu veux aller <b>loin</b>...",
        -1, 0, &attr_list, &todraw, NULL, NULL);
    pango_layout_set_attributes(layout, attr_list);
    pango_layout_set_text(layout, todraw, -1);
    g_free(todraw);
    pango_attr_list_unref(attr_list);
    pango_layout_set_font_description(layout, desc);
    cairo_set_source_rgb(cr, 0.4, 0.4, 0.4); // light gray
    pango_cairo_show_layout(cr, layout);

    cairo_translate(cr, 0, 20);
    pango_font_description_set_size(desc, 10 * PANGO_SCALE);
    pango_font_description_set_weight(desc, PANGO_WEIGHT_MEDIUM);
    pango_layout_set_font_description(layout, desc);
    pango_layout_set_attributes(layout, NULL);
    pango_layout_set_text(layout, "Marchons Ensemble!", -1);
    cairo_set_source_rgb(cr, 0, 0, 0); // black
    pango_cairo_show_layout(cr, layout);

    // wavy line
    cairo_translate(cr, 0, 22);
    cairo_move_to(cr, 0, 0);
    cairo_rel_curve_to(cr, 50, -10, 100, 10, 150, 0);
    cairo_stroke(cr);

    cairo_translate(cr, 0, 20);
    pango_layout_set_text(layout, label, -1);
    pango_font_description_set_size(desc, 8 * PANGO_SCALE);
    pango_font_description_set_weight(desc, PANGO_WEIGHT_SEMIBOLD);
    pango_font_description_set_style(desc, PANGO_STYLE_NORMAL);
    pango_layout_set_font_description(layout, desc);
    pango_layout_set_alignment(layout, PANGO_ALIGN_RIGHT);
    pango_cairo_show_layout(cr, layout);

    cairo_translate(cr, 0, 20);
    pango_layout_set_text(layout, "Mensuel de St-Adrien", -1);
    pango_font_description_set_size(desc, 6 * PANGO_SCALE);
    pango_font_description_set_weight(desc, PANGO_WEIGHT_MEDIUM);
    pango_font_description_set_style(desc, PANGO_STYLE_ITALIC);
    pango_layout_set_font_description(layout, desc);
    pango_cairo_show_layout(cr, layout);

    g_object_unref(layout);
    pango_font_description_free(desc);
}

int main(int argc, char **argv)
{
    cairo_t *cr;
    char *filename;
    cairo_status_t status;
    cairo_surface_t *surface;

    if (argc != 3)
    {
        g_printerr ("Usage: marchons <nom-du-mois> <fichier-pdf>\n");
        return 1;
    }

    filename = argv[2];

    surface = cairo_pdf_surface_create(filename, 792, 612);
    status = cairo_surface_status(surface);
    if (status != CAIRO_STATUS_SUCCESS)
    {
        g_printerr("Could not save to '%s'\n", filename);
        return 1;
    }

    cr = cairo_create(surface);

    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_paint(cr);
    draw_header(cr, argv[1]);
    cairo_destroy(cr);

    status = cairo_surface_write_to_png(surface, filename);
    cairo_surface_destroy(surface);

    return 0;
}
