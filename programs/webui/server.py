#! /usr/bin/env python3

from flask import Flask, send_from_directory, request
import json
import subprocess
import io
import math

app = Flask(__name__)
app.config.from_object(__name__)

binary = '../teragon/bin/teragon'

@app.route('/')
def root():
	return send_from_directory('static', 'index.html')

@app.route('/<path:path>')
def static_files(path):
	return send_from_directory('static', path)
	
@app.route('/compute', methods=['POST'])
def compute():
	string = request.get_data().decode(encoding='ascii')
	data = json.loads(string)
	coords = data['coords']
	darrays = "\n".join(as_darrays(coords))
	output = subprocess.check_output([binary, '-n', str(data['niter']), '-', '-'], timeout=10, input=darrays, universal_newlines=True)
	return output.replace(" ", ",")[:-2]

def as_darrays(coords):
	assert len(coords) % 2 == 0 and len(coords) >= 6
	yield "4"
	yield "{}, {}".format(coords[0], coords[1])
	yield "{}, {}".format(coords[-2], coords[-1])
	
	rescale(coords)
	yield str(len(coords) - 4)
	for index in range(2, len(coords) - 2, 2):
		yield "{}, {}".format(coords[index], coords[index+1])

def rescale(coords):
	#TODO: this should be done in C really.
	#Oh well.
	start  = coords[:2]
	points = coords[2:-2]
	end    = coords[-2:]
	adjacent = end[0] - start[0]
	opposite = end[1] - start[1]
	hypotenuse = math.hypot(opposite, adjacent)
	cos = adjacent / hypotenuse
	sin = opposite / hypotenuse
	
	for i in range(0, len(points), 2):
		points[i]   -= start[0]
		points[i+1] -= start[1]
		points[i]   /= hypotenuse
		points[i+1] /= hypotenuse
		points[i:i+2] = rotate(cos, -sin, *points[i:i+2])
		
	coords[2:-2] = points

def rotate(cos, sin, x, y):
	return (x * cos - y * sin, x * sin + y * cos)

if __name__ == "__main__":
	app.run(host='0.0.0.0', port=5000, debug=True)