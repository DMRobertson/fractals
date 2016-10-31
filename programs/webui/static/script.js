var cy = cytoscape({
	container: document.getElementById('graph'),
	elements: [
		{
			data: { id: 'start' }
		},
		{
			data: { id: 'mid' }
		},
		{
			data: { id: 'end' }
		},
		{ data: {
			id: 'e0',
			source: 'start',
			target: 'mid'
		}},
		{ data: {
			id: 'e1',
			source: 'mid',
			target: 'end'
		}}
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
	if (id === "start" || id === "end"){
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

cy.on('free position', 'node', function (event) {
	var coords = getData();
	var request = new XMLHttpRequest();
	request.open('POST', 'compute', true);
	request.setRequestHeader('Content-type', 'text/plain');
	request.onreadystatechange = function () {
		if (request.readyState === 4 && request.status === 200) {
			coords = JSON.parse('[' + request.responseText + ']');
			plot(coords);
		}
	};
	request.send(JSON.stringify(coords));
});

function resize_plot () {
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

window.addEventListener('resize', resize_plot);
window.addEventListener('DOMContentLoaded', resize_plot);
// window.addEventListener('DOMContentLoaded', make_controls);

//TODO: make dat.GUI?
// function make_controls () {
// 	var Options = function () {
// 		this.iterations = 5;
// 		this.live = false;
// 	};
// 	
// 	var options = new Options();
// 	var gui = new dat.GUI();
// 	gui.add(options, 'iterations', 2, 10).step(1);
// 	gui.add(options, 'live');
// }
