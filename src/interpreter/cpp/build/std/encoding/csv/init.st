// CSV module - native bindings
package csv;

// CSV parsing functions (native implementations)
fn parse(csvString: string) Array<Array<string>>;
fn stringify(records: Array<Array<string>>) string;
