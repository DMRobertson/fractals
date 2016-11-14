var cy = cytoscape({
	container: document.getElementById('graph'),
	elements: [
		{
			data: { id: 'start' }, 
			selected: false
		},
		{
			data: { id: 'mid' }, 
			selected: false
		},
		{
			data: { id: 'end' }, 
			selected: false
		},
		{
			data: {
				id: 'e0',
				source: 'start',
				target: 'mid'
			},
			selected: false
		},
		{
			data: {
				id: 'e1',
				source: 'mid',
				target: 'end'
			},
			selected: false
		}
	],
	style: [
		{
			selector: 'node',
			style: {
				'opacity': 0.2
			}
		},
		{
			selector: '#start, #end',
			style: {
				'shape': 'square'
			}
		},
		{
			selector: 'edge',
			style: {
				'opacity': 0.2,
				'width': 4,
				'target-arrow-shape': 'triangle',
				'curve-style': 'bezier'
			}
		}
	],
	zoomingEnabled: false,
	panningEnabled: false,
	boxSelectionEnabled: false,
	selectionType: false,
	pixelRatio: 1.0
});

cy.on('taphold', 'node', function (event) {
	var id = this.id();
	if (id === 'start' || id === 'end') {
		return;
	}
	var target = this.outgoers('node')[0].id();
	var source = this.incomers('node')[0].id();
	var node = this;
	cy.batch(function () {
		cy.remove(node);
		cy.add({
			data: {
				source: source,
				target: target
			}
		});
	});
});

cy.on('taphold', 'edge', function (event) {
	var edge = this;
	cy.batch(function () {
		cy.remove(edge);
		var newnode = cy.add({
			position: event.cyPosition
		});
		cy.add([
			{ data: {
				target: edge.target().id(),
				source: newnode.id()
			}},
			{ data: {
				source: edge.source().id(),
				target: newnode.id()
			}}
		]);
	});
	computeDimension();
	computeRequest();
});

function getData () {
	var coords = [];
	var node = cy.$('#start')[0];
	while (node !== undefined) {
		var position = node.renderedPosition();
		coords.push(position.x);
		coords.push(position.y);
		node = node.outgoers('node')[0];
	}
	return coords;
}

var computeRequest = function (event) {
	var coords = getData();
	var request = new XMLHttpRequest();
	request.open('POST', 'compute', true);
	request.setRequestHeader('Content-type', 'text/plain');
	request.onreadystatechange = function () {
		if (request.readyState === 4 && request.status === 200) {
			document.body.classList.remove('loading');
			coords = JSON.parse('[' + request.responseText + ']');
			plot(coords);
		}
	};
	request.send(JSON.stringify({
		'coords': coords,
		'niter': options.live ? Math.min(options.iterations, 5) : options.iterations,
		'flip': options.random ? 'R' : 'N'
	}));
	document.body.classList.add('loading');
};

cy.on('free', 'node', computeRequest);

var computeDimension = function () {
	var coords = getData();
	var initialLength = Math.hypot(coords[0] - coords[coords.length - 2], coords[1] - coords[coords.length - 1]);
	var ratios = [];
	for (var i = 2; i < coords.length; i += 2) {
		var hypot = Math.hypot(coords[i - 2] - coords[i], coords[i - 1] - coords[i + 1]);
		ratios.push(hypot / initialLength);
	}
	var maxRatio = 0;
	for (i = 0; i < ratios.length; i += 1) {
		if (ratios[i] >= 1) {
			document.getElementById('dimension').innerText = 'N/A';
			return;
		}
		maxRatio = Math.max(maxRatio, ratios[i]);
	}
	var lower = 0;
	var upper = -Math.log(ratios.length) / Math.log(maxRatio);
	var dim = intervalBisection(function (t) {
		var sum = -1;
		for (var i = 0; i < ratios.length; i++) {
			sum += Math.pow(ratios[i], t);
		}
		return sum;
	}, lower, upper, 0.001);
	if (dim !== undefined) {
		document.getElementById('dimension').innerText = dim.toFixed(3).toString();
	}
};

var intervalBisection = function (f, lower, upper, tolerance) {
	var error = Infinity;
	var values = [f(lower), f(upper)];
	if ((f(lower) > 0) === (f(upper) > 0)) {
		return undefined;
	}
	while (error > tolerance) {
		var mid = (lower + upper) / 2;
		var result = f(mid);
		if ((result > 0) === (values[0] > 0)) {
			values[0] = result;
			lower = mid;
		} else {
			values[1] = result;
			upper = mid;
		}
		error = (upper - lower) / 2;
	}
	return (upper + lower) / 2;
};

cy.on('position add', 'node', computeDimension);

function resizePlot () {
	var canvas = document.getElementById('plot');
	canvas.width = canvas.offsetWidth;
	canvas.height = canvas.offsetHeight;
}

function plot (coords) {
	var canvas = document.getElementById('plot');
	var ctx = canvas.getContext('2d');
	ctx.clearRect(0, 0, canvas.width, canvas.height);
	ctx.beginPath();
	ctx.moveTo(coords[0], coords[1]);
	for (var i = 2; i <= coords.length; i += 2) {
		ctx.lineTo(coords[i], coords[i + 1]);
	}
	ctx.stroke();
}

window.addEventListener('resize', resizePlot);

function toggleHelp () {
	var details = document.getElementById('details');
	if (details.style.display === 'none') {
		details.style.display = 'block';
	} else {
		details.style.display = 'none';
	}
}

window.addEventListener('DOMContentLoaded', function () {
	resizePlot();
	makeControls();
	document.getElementById('toggle_help').addEventListener('click', toggleHelp);
});

var Options = function () {
	this.iterations = 5;
	this.live = false;
	this.random = false;
};
var options = new Options();

function makeControls () {
	var gui = new dat.GUI();
	gui.add(options, 'iterations', 2, 8).step(1).onFinishChange(function () {
		cy.$('#start').trigger('free');
	});
	gui.add(options, 'live').onFinishChange(function () {
		if (options.live) {
			cy.on('position', 'node', computeRequest);
		} else {
			cy.off('position', 'node', computeRequest);
		}
	}).name('live');
	gui.add(options, 'random').onFinishChange(function () {
		if (options.random) {
			computeRequest();
		}
	});
}
