package main;



function main(){ 
 
   Path path =Path();
   File file = File();


// operations that can raise an exception are wrapped in an
// expect keyword that can be none or some 
   val flp : Expect<FileProperties> = file.open(path.home);


when(flp){

  some->{
     print(" file " + file.type); // direcory of file
  }
  none ->{
    print("eror opening file")
  }


  // network oprations exception
   Network net = Network();
val network : Excpect<NetworkResult> = net.get("http://google.com");
when(network){

  some<NetworkResult->{
     print(" network " + network.status); // direcory of file
  }
  none<Exeption> ->{
    print("eror  "+ )
  }

}
   



}