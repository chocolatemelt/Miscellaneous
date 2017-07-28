import React, { Component } from 'react'
import PropTypes from 'prop-types'

export default class AddSubreddit extends Component {
  handleChange(e) {
    this.props.handleChange(e.target.value)
  }

  handleSubmit(e) {
    e.preventDefault()
    this.props.handleSubmit(e.target.value)
  }

  render() {
    const { value, handleChange, handleSubmit } = this.props

    return(
      <form onSubmit={ this.handleSubmit }>
        <label>
          add subreddit:&nbsp;
          <input type="text" ref="addSub" value={ value } onChange={ this.handleChange } />
        </label>
        <input type="submit" value="Submit" />
      </form>
    )
  }
}

AddSubreddit.propTypes = {
  value: PropTypes.string.isRequired,
  handleChange: PropTypes.func.isRequired,
  handleSubmit: PropTypes.func.isRequired
}
