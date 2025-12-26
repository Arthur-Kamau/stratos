// CSV module - CSV parsing and writing
// Inspired by Go's encoding/csv package

package csv;

// ============================================================================
// Types
// ============================================================================

class Reader {
    var delimiter: string;
    var comment: string;
    var lazyQuotes: bool;
    var trimLeadingSpace: bool;
    var fieldsPerRecord: int;  // -1 for variable

    fn read(input: string) Result<Array<Array<string>>, Error>;
    fn readRecord(line: string) Result<Array<string>, Error>;
}

class Writer {
    var delimiter: string;
    var useCRLF: bool;

    fn write(records: Array<Array<string>>) string;
    fn writeRecord(record: Array<string>) string;
}

// ============================================================================
// Native function declarations
// ============================================================================

// Quick parse/write
fn parse(csv: string) Result<Array<Array<string>>, Error>;
fn stringify(records: Array<Array<string>>) string;

// Reader/Writer creation
fn newReader(delimiter: string) Reader;
fn newWriter(delimiter: string) Writer;

// File operations
fn parseFile(path: string) Result<Array<Array<string>>, Error>;
fn writeFile(path: string, records: Array<Array<string>>) Result<void, Error>;

// ============================================================================
// Pure Stratos implementations
// ============================================================================

// Quick parse with headers
fn parseWithHeaders(csv: string) Result<Array<Map<string, string>>, Error> {
    val records = parse(csv);

    if !records.ok() {
        return io.Result<Array<Map<string, string>>, Error> {
            value: Array<Map<string, string>>(),
            error: records.err(),
            isOk: false
        };
    }

    val rows = records.unwrap();
    if rows.length() == 0 {
        return io.Result<Array<Map<string, string>>, Error> {
            value: Array<Map<string, string>>(),
            error: null,
            isOk: true
        };
    }

    val headers = rows[0];
    var result = Array<Map<string, string>>();

    for i in 1..rows.length() {
        val row = rows[i];
        var obj = collections.newMap<string, string>();

        for j in 0..headers.length() {
            if j < row.length() {
                obj.put(headers[j], row[j]);
            }
        }

        result.add(obj);
    }

    return io.Result<Array<Map<string, string>>, Error> {
        value: result,
        error: null,
        isOk: true
    };
}

// Write with headers
fn stringifyWithHeaders(headers: Array<string>, records: Array<Map<string, string>>) string {
    var rows = Array<Array<string>>();
    rows.add(headers);

    for val record in records {
        var row = Array<string>();
        for val header in headers {
            if record.containsKey(header) {
                row.add(record.get(header));
            } else {
                row.add("");
            }
        }
        rows.add(row);
    }

    return stringify(rows);
}

// Parse TSV (tab-separated values)
fn parseTSV(tsv: string) Result<Array<Array<string>>, Error> {
    val reader = newReader("\t");
    return reader.read(tsv);
}

// Write TSV
fn stringifyTSV(records: Array<Array<string>>) string {
    val writer = newWriter("\t");
    return writer.write(records);
}

// Parse with custom delimiter
fn parseWithDelimiter(text: string, delimiter: string) Result<Array<Array<string>>, Error> {
    val reader = newReader(delimiter);
    return reader.read(text);
}

// Get column from CSV
fn getColumn(records: Array<Array<string>>, columnIndex: int) Array<string> {
    var column = Array<string>();

    for val record in records {
        if columnIndex < record.length() {
            column.add(record[columnIndex]);
        }
    }

    return column;
}

// Get column by header name
fn getColumnByName(csv: string, columnName: string) Result<Array<string>, Error> {
    val data = parseWithHeaders(csv);

    if !data.ok() {
        return io.Result<Array<string>, Error> {
            value: Array<string>(),
            error: data.err(),
            isOk: false
        };
    }

    var column = Array<string>();
    for val row in data.unwrap() {
        if row.containsKey(columnName) {
            column.add(row.get(columnName));
        }
    }

    return io.Result<Array<string>, Error> {
        value: column,
        error: null,
        isOk: true
    };
}

// Filter CSV rows
fn filterRows(records: Array<Array<string>>, predicate: Function<Array<string>, bool>) Array<Array<string>> {
    var filtered = Array<Array<string>>();

    for val record in records {
        if predicate(record) {
            filtered.add(record);
        }
    }

    return filtered;
}

// Map CSV rows
fn mapRows(records: Array<Array<string>>, fn: Function<Array<string>, Array<string>>) Array<Array<string>> {
    var mapped = Array<Array<string>>();

    for val record in records {
        mapped.add(fn(record));
    }

    return mapped;
}

// Transpose CSV (swap rows and columns)
fn transpose(records: Array<Array<string>>) Array<Array<string>> {
    if records.length() == 0 {
        return Array<Array<string>>();
    }

    val numCols = records[0].length();
    var transposed = Array<Array<string>>();

    for col in 0..numCols {
        var newRow = Array<string>();
        for val record in records {
            if col < record.length() {
                newRow.add(record[col]);
            } else {
                newRow.add("");
            }
        }
        transposed.add(newRow);
    }

    return transposed;
}

// Merge two CSV files (by columns)
fn mergeByColumns(records1: Array<Array<string>>, records2: Array<Array<string>>) Array<Array<string>> {
    var merged = Array<Array<string>>();
    val maxRows = math.max(records1.length(), records2.length());

    for i in 0..maxRows {
        var row = Array<string>();

        if i < records1.length() {
            for val cell in records1[i] {
                row.add(cell);
            }
        }

        if i < records2.length() {
            for val cell in records2[i] {
                row.add(cell);
            }
        }

        merged.add(row);
    }

    return merged;
}

// Convert CSV to JSON
fn toJSON(csv: string) Result<json.JsonValue, Error> {
    val data = parseWithHeaders(csv);

    if !data.ok() {
        return io.Result<json.JsonValue, Error> {
            value: null,
            error: data.err(),
            isOk: false
        };
    }

    var jsonArray = Array<json.JsonValue>();

    for val row in data.unwrap() {
        val obj = collections.newMap<string, json.JsonValue>();

        row.keys().forEach((key) => {
            obj.put(key, json.jsonString(row.get(key)));
        });

        jsonArray.add(json.jsonObject(obj));
    }

    return io.Result<json.JsonValue, Error> {
        value: json.jsonArray(jsonArray),
        error: null,
        isOk: true
    };
}

// Convert JSON to CSV
fn fromJSON(jsonData: json.JsonValue) Result<string, Error> {
    if !jsonData.isArray() {
        return io.Result<string, Error> {
            value: "",
            error: io.Error("JSON data must be an array"),
            isOk: false
        };
    }

    val array = jsonData.asArray();
    if array.length() == 0 {
        return io.Result<string, Error> {
            value: "",
            error: null,
            isOk: true
        };
    }

    // Extract headers from first object
    val firstObj = array[0];
    if !firstObj.isObject() {
        return io.Result<string, Error> {
            value: "",
            error: io.Error("Array elements must be objects"),
            isOk: false
        };
    }

    val headers = firstObj.asObject().keys();
    var records = Array<Map<string, string>>();

    for val item in array {
        if item.isObject() {
            val obj = collections.newMap<string, string>();
            val itemObj = item.asObject();

            headers.forEach((key) => {
                val value = itemObj.get(key);
                if value != null {
                    obj.put(key, value.asString());
                }
            });

            records.add(obj);
        }
    }

    return io.Result<string, Error> {
        value: stringifyWithHeaders(headers.items, records),
        error: null,
        isOk: true
    };
}
