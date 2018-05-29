require 'finalize_block.so'
require "finalize_block/version"

class FinalizeBlock
  def initialize rules
    raise "Now, one rule should be accepted: #{rules.inspect}" if rules.keys.size != 1
    rules.each{|k, v| @klass = k; @action = v}
    @traces = setup_traces @klass
    @traces.each{|trace| trace.enable}
    yield
  ensure
    @traces.each{|trace| trace.disable}

    each_target_objects do |obj|
      @action.call(obj)
    end
  end
end

def finalize_block rules, &b
  FinalizeBlock.new rules, &b
end
