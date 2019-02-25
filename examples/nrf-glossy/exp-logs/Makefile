plots-pdf:
	#if [ -f plots/*.pdf ]; then for f in plots/*.pdf; do echo "Cropping $$f"; pdfcrop $$f $$f; done fi
	#for f in plots/*.pdf; do if [[ -f plots/*.pdf ]]; then echo "Cropping $$f"; pdfcrop $$f $$f; fi; done
	for f in *.pdf; do echo "Cropping $$f"; pdfcrop $$f $$f; done

preview:
	open -a Preview *.pdf 

all: plots-pdf preview

