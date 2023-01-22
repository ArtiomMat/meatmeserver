package ai

type nUnit struct {
	inputs  []float32
	weights []float32
	bias    float32

	output float32
}

func (n nUnit) activate() float32 {
	n.output = n.bias
	for i, x := range n.inputs {
		n.output += n.weights[i] * x
	}

	return n.output
}
