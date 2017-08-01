import React, { Component } from 'react'
import PropTypes from 'prop-types'
import { connect } from 'react-redux'
import {
  addSubreddit,
  selectSubreddit,
  fetchPostsIfNeeded,
  invalidateSubreddit
} from '../actions'
import AddSubreddit from '../containers/AddSubreddit'
import Posts from '../components/Posts'
import Select from '../components/Select'

class AsyncApp extends Component {
  constructor(props) {
    super(props)

    this.handleChange = this.handleChange.bind(this)
    this.handleRefresh = this.handleRefresh.bind(this)
  }

  handleChange(subreddit) {
    this.props.dispatch(selectSubreddit(subreddit))
    this.props.dispatch(fetchPostsIfNeeded(subreddit))
  }

  handleRefresh(e) {
    e.preventDefault()

    const { dispatch, selectedSubreddit } = this.props
    dispatch(invalidateSubreddit(selectedSubreddit))
    dispatch(fetchPostsIfNeeded(selectedSubreddit))
  }

  render() {
    return(
      <div>
        <Select
          value={this.props.selectedSubreddit}
          onChange={this.handleChange}
          options={[]}
        />
        <AddSubreddit onSubmit={this.handleSubmit} />
        <Posts />
      </div>
    )
  }
}

AsyncApp.propTypes = {
  selectedSubreddit: PropTypes.string,
  posts: PropTypes.array,
  isFetching: PropTypes.bool,
  lastUpdated: PropTypes.number,
  dispatch: PropTypes.func.isRequired
}

function mapStateToProps(state) {
  const { selectedSubreddit, postsBySubreddit } = state
  const {
    isFetching,
    lastUpdated,
    items: posts
  } = postsBySubreddit[selectedSubreddit] || {
    isFetching: true,
    items: []
  }

  return {
    selectedSubreddit,
    posts,
    isFetching,
    lastUpdated
  }
}

export default connect(mapStateToProps)(AsyncApp)
