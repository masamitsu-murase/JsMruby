
# JsMruby - mruby NPAPI plugin for Firefox and Chrome

## Sample

First of all, I show sample code.

Install [JsMruby plugin](http://sdrv.ms/WGpMpE) and see [demonstration](http://masamitsu-murase.github.com/JsMruby/canvas_sample.html).  
If you use Firefox, please use `jsmruby.xpi`. 
If you use Google Chrome,  please download `jsmruby_chrome.zip` and extract it, then do `load unpackaged extensions`. 

The current version 0.1.0 supports the following platform:

* Firefox and Chrome on Windows.
* Firefox and Chrome on Mac OS X (Intel).  
* Firefox on Ubuntu Linux.  
  I have not checked on Google Chrome on Ubuntu.

```html
<!DOCTYPE html>
<html>
<head><title>Sample</title></head>
<body>

<!-- Load mruby plugin -->
<object id="jsmruby" type="application/x-jsmruby-plugin" width="1" height="1" style="position: absolute;"></object>

<script id="mruby_script" type="text/ruby">
#<![CDATA[
class Canvas
  attr_accessor :color

  def initialize(id)
    doc = JsObj.get("document")
    @canvas = doc.getElementById(id)
    @color = "rgb(0, 0, 0)"
  end

  def draw_rect(x, y, size)
    ctx = @canvas.getContext("2d")

    ctx.beginPath
    ctx.fillStyle = @color
    ctx.fillRect(x, y, size, size)
  end
end

def main
  c = Canvas.new("canvas_id")
  c.color = "rgb(255, 0, 0)"
  c.draw_rect(10, 20, 30)
end
# ]]>
</script>

<script type="text/javascript">
  var jsmruby = document.getElementById("jsmruby");
  var text = document.getElementById("mruby_script").firstChild.nodeValue;
  jsmruby.load(text);  // Load mruby script
</script>

<h1>Sample</h1>
<div><canvas id="canvas_id" height="80" width="80" style="border: 1px solid black;"></canvas></div>

<form>
  <input type=button value="Draw" onclick='jsmruby.send("main")' />
</form>

</body>
</html>
```

