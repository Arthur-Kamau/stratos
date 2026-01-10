package std.fs

// Native C bindings
extern fn fopen(filename: string, mode: string) any; // returns FILE*
extern fn fclose(stream: any) int;
extern fn fputs(str: string, stream: any) int;

class File {
    var handle: any; // FILE*
    var path: string;

    constructor(p: string) {
        this.path = p;
        this.handle = 0; // null
    }

    pub fn open(mode: string) {
        this.handle = fopen(this.path, mode);
    }
    
    pub fn close() {
        if (this.handle != 0) {
            fclose(this.handle);
            this.handle = 0;
        }
    }
    
    pub fn write(text: string) {
        if (this.handle != 0) {
            fputs(text, this.handle);
        }
    }
}