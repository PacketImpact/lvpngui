#!/usr/bin/env python3
import os
import markdown

layout = """<!DOCTYPE html>
<html>
<head>
	<title>Changelog</title>
	<style type="text/css">{style}</style>
</head>
<body>{html}</body>
</html>
"""

style = """
body { background: #303030; color: #ffbf00; font-family: Consolas, "Courier New", monospace, Courier; }
h1, h2, h3, h4, h5, h6, a, a:visited {
	color: #eee;
}
h2 { margin-bottom: 0.5em; }
h3 { margin-left: 0.75em; }
h3, ul, p { margin-bottom: 0.25em; margin-top: 0.25em; }
ul { list-style-type: circle; }

p { margin-left: 0.75em; }
h1 + p { margin-left: 0; }
"""

root = os.path.dirname(os.path.abspath(__file__))
i_path = os.path.join(root, 'CHANGELOG.md')
o_path = os.path.join(root, 'CHANGELOG.html')

with open(i_path, 'r') as i:
	with open(o_path, 'w') as o:
		text = i.read()
		html = markdown.markdown(text)
		o.write(layout.format(html=html, style=style))
		
print("saved " + o_path)