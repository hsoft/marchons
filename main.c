#include <math.h>
#include <pango/pangocairo.h>
#include <cairo-pdf.h>

#define SECTION_WIDTH 170
#define PAGE_MARGIN 25

static void
convert_markup(char *dest, const char *src)
{
    int bcount = 0;
    int icount = 0;

    while (*src) {
        char c = *src;
        if (c == '*') {
            *dest = '<'; dest++;
            if (bcount % 2) {
                *dest = '/'; dest++;
            }
            *dest = 'b'; dest++;
            *dest = '>'; dest++;
            bcount++;
        } else if (c == '_') {
            *dest = '<'; dest++;
            if (icount % 2) {
                *dest = '/'; dest++;
            }
            *dest = 'i'; dest++;
            *dest = '>'; dest++;
            icount++;
        } else {
            *dest = c; dest++;
        }
        src++;
    }
    *dest = '\0';
}

static void
draw_header(cairo_t *cr, char *label)
{
    PangoLayout *layout;
    PangoFontDescription *desc;
    PangoAttrList *attr_list;
    char *todraw;

    cairo_save(cr);

    layout = pango_cairo_create_layout(cr);
    pango_layout_set_width(layout, SECTION_WIDTH * PANGO_SCALE);

    desc = pango_font_description_from_string("Sans Italic 6");
    cairo_translate(cr, PAGE_MARGIN, PAGE_MARGIN);
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
    cairo_rel_curve_to(cr, 50, -10, 100, 10, SECTION_WIDTH, 0);
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

    cairo_restore(cr);
}

static void
draw_section(cairo_t *cr, FILE *fp, int x, int y, int height)
{
    cairo_save(cr);
    cairo_translate(cr, x, y);
    cairo_set_line_width(cr, 1);
    cairo_set_source_rgb(cr, 0.4, 0.4, 0.4); // light gray
    cairo_move_to(cr, 0, height);
    cairo_line_to(cr, 0, 0);
    cairo_line_to(cr, SECTION_WIDTH, 0);
    cairo_stroke(cr);

    PangoLayout *layout = pango_cairo_create_layout(cr);
    PangoFontDescription *desc = pango_font_description_from_string("Sans Bold 8");
    pango_layout_set_font_description(layout, desc);
    cairo_set_source_rgb(cr, 0, 0, 0); // black

    char *line = NULL;
    size_t len = 0;

    if (getline(&line, &len, fp) != -1) {
        cairo_translate(cr, 10, 10);
        pango_layout_set_text(layout, line, -1);
        pango_cairo_show_layout(cr, layout);
    }

    cairo_translate(cr, 0, 20);
    pango_font_description_set_size(desc, 6 * PANGO_SCALE);
    pango_font_description_set_weight(desc, PANGO_WEIGHT_MEDIUM);
    pango_layout_set_font_description(layout, desc);
    pango_layout_set_width(layout, SECTION_WIDTH * PANGO_SCALE);

    while (getline(&line, &len, fp) != -1) {
        if (strncmp(line, "---", 3) == 0) {
            break;
        }
        PangoAttrList *attr_list;
        char converted[1024];
        char *todraw;
        convert_markup(converted, line);
        pango_parse_markup(
            converted, -1, 0, &attr_list, &todraw, NULL, NULL);
        pango_layout_set_attributes(layout, attr_list);
        pango_layout_set_text(layout, todraw, -1);
        pango_cairo_show_layout(cr, layout);
        // On fait -1 parce que la ligne se termine toujours par un \n qui
        // augmente artificiellement le line count.
        int lines = pango_layout_get_line_count(layout) - 1;
        cairo_translate(cr, 0, 10 * lines);
        g_free(todraw);
    }

    g_object_unref(layout);
    pango_font_description_free(desc);
    cairo_restore(cr);
}

int main(int argc, char **argv)
{
    cairo_t *cr;
    char *fnin, *fnout;
    char label[255] = {0};
    cairo_status_t status;
    cairo_surface_t *surface;

    if (argc != 3)
    {
        g_printerr("Usage: marchons <fichier-source> <fichier-pdf>\n");
        return 1;
    }

    fnin = argv[1];
    fnout = argv[2];

    FILE *fp = fopen(fnin, "r");
    if (fp == NULL) {
        g_printerr("Could not open '%s'\n", fnin);
        return 1;
    }

    // Extract label from filename
    char *s = strrchr(fnin, '/');
    if (s == NULL) {
        s = fnin;
    } else {
        s++;
    }
    strcpy(label, s);
    s = strrchr(label, '.');
    if (s != NULL) {
        *s = '\0';
    }

    surface = cairo_pdf_surface_create(fnout, 792, 612);
    status = cairo_surface_status(surface);
    if (status != CAIRO_STATUS_SUCCESS)
    {
        g_printerr("Could not save to '%s'\n", fnout);
        return 1;
    }

    cr = cairo_create(surface);

    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_paint(cr);
    draw_header(cr, label);
    draw_section(cr, fp, PAGE_MARGIN, 120, 470);

    for (int i=1; i<4; i++) {
        draw_section(cr, fp, 190*i+PAGE_MARGIN, PAGE_MARGIN, 270);
        draw_section(cr, fp, 190*i+PAGE_MARGIN, 320, 270);
    }
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
    fclose(fp);

    return 0;
}
