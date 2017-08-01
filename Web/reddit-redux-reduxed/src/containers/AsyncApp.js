import React, { Component } from 'react'
import PropTypes from 'prop-types'
import { connect } from 'react-redux'
import {
} from '../actions'
import AddSubreddit from '../components/AddSubreddit'
import Posts from '../components/Posts'
import Select from '../components/Select'

class AsyncApp extends Component {
  constructor(props) {
    super(props)
  }

  render() {
    return(
      <div>
        <Select />
        <AddSubreddit />
        <Posts />
      </div>
    )
  }
}

AsyncApp.propTypes = {
}

function mapStateToProps(state) {
}

export default connect(mapStateToProps)(AsyncApp)
