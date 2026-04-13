// Tag Dispatch Pattern in C
// Select behavior at compile/link time using function pointers keyed by tag

#include <stdio.h>
#include <string.h>

typedef enum { FMT_JSON, FMT_XML, FMT_CSV } Format;

typedef struct { const char* key; const char* value; } KV;

void serialize_json(const KV* data, int n, char* out, int out_size) {
    int pos = 0;
    pos += snprintf(out + pos, out_size - pos, "{");
    for (int i = 0; i < n; i++) {
        if (i > 0) pos += snprintf(out + pos, out_size - pos, ",");
        pos += snprintf(out + pos, out_size - pos, "\"%s\":\"%s\"", data[i].key, data[i].value);
    }
    snprintf(out + pos, out_size - pos, "}");
}

void serialize_xml(const KV* data, int n, char* out, int out_size) {
    int pos = 0;
    pos += snprintf(out + pos, out_size - pos, "<record>");
    for (int i = 0; i < n; i++)
        pos += snprintf(out + pos, out_size - pos, "<%s>%s</%s>", data[i].key, data[i].value, data[i].key);
    snprintf(out + pos, out_size - pos, "</record>");
}

void serialize_csv(const KV* data, int n, char* out, int out_size) {
    int pos = 0;
    for (int i = 0; i < n; i++)
        pos += snprintf(out + pos, out_size - pos, "%s%s", i ? "," : "", data[i].key);
    pos += snprintf(out + pos, out_size - pos, "\n");
    for (int i = 0; i < n; i++)
        pos += snprintf(out + pos, out_size - pos, "%s%s", i ? "," : "", data[i].value);
}

typedef void (*Serializer)(const KV*, int, char*, int);

Serializer get_serializer(Format fmt) {
    switch (fmt) {
        case FMT_JSON: return serialize_json;
        case FMT_XML: return serialize_xml;
        case FMT_CSV: return serialize_csv;
    }
    return serialize_json;
}

int main(void) {
    KV data[] = {{"name", "Alice"}, {"age", "30"}, {"city", "Seoul"}};
    int n = 3;
    char buf[256];

    Format formats[] = {FMT_JSON, FMT_XML, FMT_CSV};
    const char* names[] = {"JSON", "XML", "CSV"};

    for (int i = 0; i < 3; i++) {
        Serializer s = get_serializer(formats[i]);
        s(data, n, buf, sizeof(buf));
        printf("%s: %s\n", names[i], buf);
    }
    return 0;
}
