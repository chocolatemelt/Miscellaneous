import React, { Component } from 'react'
import PropTypes from 'prop-types'

export default class Select extends Component {
  render() {
    const { value, onChange, options } = this.props

    if(options) {
      return(
        <div>
          <h1>{value}</h1>
          <select onChange={e => onChange(e.target.value)} value={value}>
            {options.map(option => (
              <option value={option} key={option}>{option}</option>
            ))}
          </select>
        </div>
      )
    }
    else {
      return(<h1>nothing yet...</h1>)
    }
  }
}

Select.propTypes = {
  options: PropTypes.arrayOf(PropTypes.string.isRequired).isRequired,
  value: PropTypes.string.isRequired,
  onChange: PropTypes.func.isRequired
}
