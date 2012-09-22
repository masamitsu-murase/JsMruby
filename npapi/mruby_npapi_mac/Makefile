
CC = gcc
CXX = g++
CFLAGS = -Wall -O3 -fPIC -DXP_MACOSX=1 -arch i386
LFLAGS = -bundle -arch i386
INCLUDE = -I../include -I../sdk -I../../../mruby/include
LIBS = ../../../mruby/lib/libmruby.a
OBJS = mrb_js.o np_entry.o npp_gate.o mrb_js_object.o npn_gate.o plugin.o


all: npjsmrby_bundle


npjsmrby_bundle: npjsmrby_bundle_dir npjsmrby
	cp -f npjsmrby npjsmrby.bundle/Contents/MacOS/
	cp -f Info.plist npjsmrby.bundle/Contents/
	cp -f -R English.lproj npjsmrby.bundle/Contents/Resources/

npjsmrby_bundle_dir:
	mkdir -p npjsmrby.bundle
	mkdir -p npjsmrby.bundle/Contents/Resources
	mkdir -p npjsmrby.bundle/Contents/MacOS

npjsmrby: $(OBJS)
	$(CXX) $(CFLAGS) $(LFLAGS) $(OBJS) -o $@ $(LIBS)

$(OBJS) : %.o : ../src/%.cpp
	$(CXX) $(INCLUDE) $(CFLAGS) -c $< -o $@


clean:
	rm -f -r $(OBJS) npjsmrby.bundle

# task :default => :build
# desc 'Build loadable bundle'
# task :build => ['Hello'] do |t|
#   bundle_name = 'build/Hello.bundle'
#   Dir.mkdir bundle_name
#   Dir.mkdir "#{bundle_name}/Contents"
#   Dir.mkdir "#{bundle_name}/Contents/Resources"
#   Dir.mkdir "#{bundle_name}/Contents/MacOS"
#   sh "cp Info.plist #{bundle_name}/Contents"
#   sh "cp -R English.lproj #{bundle_name}/Contents/Resources"
#   sh "cp Hello #{bundle_name}/Contents/MacOS"
# end
# 
# file 'Hello' => ['Hello.ppc', 'Hello.i386'] do |t|
#   sh "/usr/bin/lipo -create #{t.prerequisites} -output #{t}"
# end
# 
# file 'Hello.ppc' => ['Hello.ppc.o'] do |t|
#   sh "/usr/bin/gcc-4.0 -o #{t} -framework Cocoa -arch ppc -filelist #{t}.LinkFileList -bundle -mmacosx-version-min=10.4"
# end
# 
# file 'Hello.i386' => ['Hello.i386.o'] do |t|
#   sh "/usr/bin/gcc-4.0 -o #{t} -framework Cocoa -arch i386 -filelist #{t}.LinkFileList -bundle -mmacosx-version-min=10.4"
# end
# 
# file 'Hello.ppc.o' => ['Hello.m'] do |t|
#   sh "/usr/bin/gcc-4.0 -x objective-c -arch ppc -o #{t} -c #{t.prerequisites}"
#   File.open('Hello.ppc.LinkFileList', 'w') { |f| f.write('Hello.ppc.o') }
# end

