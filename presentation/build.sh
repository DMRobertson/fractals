mkdir -p build

latex="pdflatex -halt-on-error -interaction=nonstopmode -output-directory=build"
show_errors="egrep -n --color=auto -B2 -A3 ^!"

if [[ "$1" != "quick" ]]
then
	{ $latex      fractals | $show_errors; } ||
	{ biber build/fractals | $show_errors; } ||
	{ $latex fractals      | $show_errors; } || echo "✓"
else
	$latex fractals | $show_errors || echo "✓"

fi
