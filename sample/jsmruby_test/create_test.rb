
files = Dir.glob("#{File.dirname(__FILE__)}/../../mruby/test/t/*.rb").to_a.sort
core_files = Dir.glob("#{File.dirname(__FILE__)}/../../mruby/test/*.rb").to_a.sort

File.open(File.expand_path("../jsmruby_test.html.rb", __FILE__), "w") do |output|
  files.each_with_index do |file, index|
    File.open(file) do |input|
      str = <<"EOS"
<script id="mruby_script_#{index}" type="text/ruby">
#<![CDATA[
  #{input.read}
#]]>
</script>
EOS
      output.puts(str)
    end
  end


  str = <<'EOS'
<script id="mruby_script_main" type="text/ruby">
#<![CDATA[
$ok_test = 0
$ko_test = 0
$kill_test = 0
$asserts  = []
$test_start = Time.now if Object.const_defined?(:Time)

##
# Create the assertion in a readable way
def assertion_string(err, str, iso=nil, e=nil)
  msg = "#{err}#{str}"
  msg += " [#{iso}]" if iso && iso != ''
  msg += " => #{e.message}" if e
  msg += " (mrbgems: #{GEMNAME})" if Object.const_defined?(:GEMNAME)
  msg
end

##
# Verify a code block.
#
# str : A remark which will be printed in case
#       this assertion fails
# iso : The ISO reference code of the feature
#       which will be tested by this
#       assertion
$output = JsObj.get("window.output");
def assert(str = 'Assertion failed', iso = '')
  begin
    if(!yield)
      $asserts.push(assertion_string('Fail: ', str, iso))
      $ko_test += 1
#      print('F')
      $output.call(str, 'F')
    else
      $ok_test += 1
#      print('.')
      $output.call(str, 'T')
    end
  rescue Exception => e
    $asserts.push(assertion_string('Error: ', str, iso, e))
    $kill_test += 1
#    print('X')
    $output.call(str, 'X')
  end
end

##
# Report the test result and print all assertions
# which were reported broken.
def report()
#  print "\n"

  str = ""

  $asserts.each do |msg|
    str += msg
  end

  $total_test = $ok_test.+($ko_test)
  str += ('Total: ').to_s
  str += ($total_test).to_s
  str += ("\n").to_s

  str += ('   OK: ').to_s
  str += ($ok_test).to_s
  str += ("\n").to_s
  str += ('   KO: ').to_s
  str += ($ko_test).to_s
  str += ("\n").to_s
  str += ('Crash: ').to_s
  str += ($kill_test).to_s
  str += ("\n").to_s

  if Object.const_defined?(:Time)
    str += (' Time: ').to_s
    str += (Time.now - $test_start).to_s
    str += (" seconds\n").to_s
  end

  JsObj.get("window").alert(str);
end

##
# Performs fuzzy check for equality on methods returning floats
# on the basis of the Math::TOLERANCE constant.
def check_float(a, b)
  tolerance = Math::TOLERANCE
  a = a.to_f
  b = b.to_f
  if a.finite? and b.finite?
    (a-b).abs < tolerance
  else
    true
  end
end
#]]>
</script>
EOS
  output.puts(str)


  str = <<"EOS"
<script type="text/javascript">
//<![CDATA[
  var jsmruby = document.getElementById("jsmruby");
  var text_main = document.getElementById("mruby_script_main").firstChild.nodeValue
  var text_list = [];
  for (var i=0; i<#{files.size}; i++){
    text_list.push(document.getElementById("mruby_script_" + i).firstChild.nodeValue);
  }

  // Load mruby script.
  document.getElementById("run_button").addEventListener("click", function(e){
      // Call 'main' method of mruby script.
      jsmruby.load(text_main);
      var run_test = function(){
          var text = text_list.shift();
          if (text){
              jsmruby.load(text);
              setTimeout(run_test, 0);
          }else{
              jsmruby.send("report");
          }
      };
      setTimeout(run_test, 0);
  });
//]]>
</script>
EOS
  output.puts(str)
end

