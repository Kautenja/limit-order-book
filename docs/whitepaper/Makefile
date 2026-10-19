# Build the manuscript using checked-in evidence; do not rerun experiments.
BUILD := $(abspath .build)
LATEXMK ?= latexmk
# BibTeX runs inside .build; resolve the canonical entry from the repository.
export BIBINPUTS := $(abspath ../..):$(BIBINPUTS):

.PHONY: all data clean

all:
	mkdir -p "$(BUILD)"
	$(LATEXMK) -pdf -interaction=nonstopmode -halt-on-error -file-line-error \
		-pdflatex='pdflatex -no-shell-escape %O %S' \
		-jobname=limit-order-book -outdir="$(BUILD)" main.tex

data:
	python3 evaluate.py --summarize evidence

clean:
	$(LATEXMK) -C -jobname=limit-order-book -outdir="$(BUILD)" main.tex
